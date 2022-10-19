/*
 * Copyright (c) 2011 Patrick McHardy <kaber@trash.net>
 *
 * Based on Rusty Russell's IPv4 DNAT target. Development of IPv6 NAT
 * funded by Astaro.
 */

#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <xtables.h>
#include <iptables.h> /* get_kernel_version */
#include <limits.h> /* INT_MAX in ip_tables.h */
#include <arpa/inet.h>
#include <linux/netfilter_ipv4/ip_tables.h>
#include <linux/netfilter_ipv6/ip6_tables.h>
#include <linux/netfilter/nf_nat.h>

#define TO_IPV4_MRC(ptr) ((const struct nf_nat_ipv4_multi_range_compat *)(ptr))
#define RANGE2_INIT_FROM_IPV4_MRC(ptr) {			\
	.flags		= TO_IPV4_MRC(ptr)->range[0].flags,	\
	.min_addr.ip	= TO_IPV4_MRC(ptr)->range[0].min_ip,	\
	.max_addr.ip	= TO_IPV4_MRC(ptr)->range[0].max_ip,	\
	.min_proto	= TO_IPV4_MRC(ptr)->range[0].min,	\
	.max_proto	= TO_IPV4_MRC(ptr)->range[0].max,	\
};

enum {
	O_TO_DEST = 0,
	O_TO_PORTS,
	O_RANDOM,
	O_PERSISTENT,
	F_TO_DEST  = 1 << O_TO_DEST,
	F_TO_PORTS = 1 << O_TO_PORTS,
	F_RANDOM   = 1 << O_RANDOM,
};

static void DNAT_help(void)
{
	printf(
"DNAT target options:\n"
" --to-destination [<ipaddr>[-<ipaddr>]][:port[-port]]\n"
"				Address to map destination to.\n"
"[--random] [--persistent]\n");
}

static void DNAT_help_v2(void)
{
	printf(
"DNAT target options:\n"
" --to-destination [<ipaddr>[-<ipaddr>]][:port[-port[/port]]]\n"
"				Address to map destination to.\n"
"[--random] [--persistent]\n");
}

static void REDIRECT_help(void)
{
	printf(
"REDIRECT target options:\n"
" --to-ports <port>[-<port>]\n"
"				Port (range) to map to.\n"
" [--random]\n");
}

static const struct xt_option_entry DNAT_opts[] = {
	{.name = "to-destination", .id = O_TO_DEST, .type = XTTYPE_STRING,
	 .flags = XTOPT_MAND},
	{.name = "random", .id = O_RANDOM, .type = XTTYPE_NONE},
	{.name = "persistent", .id = O_PERSISTENT, .type = XTTYPE_NONE},
	XTOPT_TABLEEND,
};

static const struct xt_option_entry REDIRECT_opts[] = {
	{.name = "to-ports", .id = O_TO_PORTS, .type = XTTYPE_STRING},
	{.name = "random", .id = O_RANDOM, .type = XTTYPE_NONE},
	XTOPT_TABLEEND,
};

/* Parses ports */
static void
parse_ports(const char *arg, bool portok, struct nf_nat_range2 *range)
{
	unsigned int port, maxport, baseport;
	char *end = NULL;

	if (!portok)
		xtables_error(PARAMETER_PROBLEM,
			      "Need TCP, UDP, SCTP or DCCP with port specification");

	range->flags |= NF_NAT_RANGE_PROTO_SPECIFIED;

	if (!xtables_strtoui(arg, &end, &port, 0, UINT16_MAX)) {
		port = xtables_service_to_port(arg, NULL);
		if (port == (unsigned)-1)
			xtables_error(PARAMETER_PROBLEM,
				      "Port `%s' not valid", arg);
		end = "";
	}

	switch (*end) {
	case '\0':
		range->min_proto.tcp.port
			= range->max_proto.tcp.port
			= htons(port);
		return;
	case '-':
		arg = end + 1;
		break;
	case ':':
		xtables_error(PARAMETER_PROBLEM,
			      "Invalid port:port syntax - use dash");
	default:
		xtables_error(PARAMETER_PROBLEM,
			      "Garbage after port value: `%s'", end);
	}

	/* it is a range, don't allow service names here */
	if (!xtables_strtoui(arg, &end, &maxport, 0, UINT16_MAX))
		xtables_error(PARAMETER_PROBLEM, "Port `%s' not valid", arg);

	if (maxport < port)
		/* People are stupid. */
		xtables_error(PARAMETER_PROBLEM,
			   "Port range `%s' funky", arg);

	range->min_proto.tcp.port = htons(port);
	range->max_proto.tcp.port = htons(maxport);

	switch (*end) {
	case '\0':
		return;
	case '/':
		arg = end + 1;
		break;
	default:
		xtables_error(PARAMETER_PROBLEM,
			      "Garbage after port range: `%s'", end);
	}

	if (!xtables_strtoui(arg, &end, &baseport, 1, UINT16_MAX)) {
		baseport = xtables_service_to_port(arg, NULL);
		if (baseport == (unsigned)-1)
			xtables_error(PARAMETER_PROBLEM,
				      "Port `%s' not valid", arg);
	}

	range->flags |= NF_NAT_RANGE_PROTO_OFFSET;
	range->base_proto.tcp.port = htons(baseport);
}

/* Ranges expected in network order. */
static void
parse_to(const char *orig_arg, bool portok,
	 struct nf_nat_range2 *range, int family)
{
	char *arg, *start, *end, *colon, *dash;

	arg = xtables_strdup(orig_arg);
	start = strchr(arg, '[');
	if (!start) {
		start = arg;
		/* Lets assume one colon is port information.
		 * Otherwise its an IPv6 address */
		colon = strchr(arg, ':');
		if (colon && strchr(colon + 1, ':'))
			colon = NULL;
	} else {
		start++;
		end = strchr(start, ']');
		if (end == NULL || family == AF_INET)
			xtables_error(PARAMETER_PROBLEM,
				      "Invalid address format");

		*end = '\0';
		colon = strchr(end + 1, ':');
	}

	if (colon) {
		parse_ports(colon + 1, portok, range);

		/* Starts with colon or [] colon? No IP info...*/
		if (colon == arg || colon == arg + 2) {
			free(arg);
			return;
		}
		*colon = '\0';
	}

	range->flags |= NF_NAT_RANGE_MAP_IPS;
	dash = strchr(start, '-');
	if (colon && dash && dash > colon)
		dash = NULL;

	if (dash)
		*dash = '\0';

	if (!inet_pton(family, start, &range->min_addr))
		xtables_error(PARAMETER_PROBLEM,
			      "Bad IP address \"%s\"", arg);
	if (dash) {
		if (!inet_pton(family, dash + 1, &range->max_addr))
			xtables_error(PARAMETER_PROBLEM,
				      "Bad IP address \"%s\"", dash + 1);
	} else {
		range->max_addr = range->min_addr;
	}
	free(arg);
	return;
}

static void __DNAT_parse(struct xt_option_call *cb, __u16 proto,
			 struct nf_nat_range2 *range, int family)
{
	bool portok = proto == IPPROTO_TCP ||
		      proto == IPPROTO_UDP ||
		      proto == IPPROTO_SCTP ||
		      proto == IPPROTO_DCCP ||
		      proto == IPPROTO_ICMP;

	xtables_option_parse(cb);
	switch (cb->entry->id) {
	case O_TO_DEST:
		parse_to(cb->arg, portok, range, family);
		break;
	case O_TO_PORTS:
		parse_ports(cb->arg, portok, range);
		break;
	case O_PERSISTENT:
		range->flags |= NF_NAT_RANGE_PERSISTENT;
		break;
	}
}

static void DNAT_parse(struct xt_option_call *cb)
{
	struct nf_nat_ipv4_multi_range_compat *mr = (void *)cb->data;
	const struct ipt_entry *entry = cb->xt_entry;
	struct nf_nat_range2 range = {};

	__DNAT_parse(cb, entry->ip.proto, &range, AF_INET);

	switch (cb->entry->id) {
	case O_TO_DEST:
		mr->range->min_ip = range.min_addr.ip;
		mr->range->max_ip = range.max_addr.ip;
		/* fall through */
	case O_TO_PORTS:
		mr->range->min = range.min_proto;
		mr->range->max = range.max_proto;
		/* fall through */
	case O_PERSISTENT:
		mr->range->flags |= range.flags;
		break;
	}
}

static void __DNAT_fcheck(struct xt_fcheck_call *cb, unsigned int *flags)
{
	static const unsigned int redir_f = F_TO_PORTS | F_RANDOM;
	static const unsigned int dnat_f = F_TO_DEST | F_RANDOM;

	if ((cb->xflags & redir_f) == redir_f ||
	    (cb->xflags & dnat_f) == dnat_f)
		*flags |= NF_NAT_RANGE_PROTO_RANDOM;
}

static void DNAT_fcheck(struct xt_fcheck_call *cb)
{
	struct nf_nat_ipv4_multi_range_compat *mr = cb->data;

	mr->rangesize = 1;

	if (mr->range[0].flags & NF_NAT_RANGE_PROTO_OFFSET)
		xtables_error(PARAMETER_PROBLEM,
			      "Shifted portmap ranges not supported with this kernel");

	__DNAT_fcheck(cb, &mr->range[0].flags);
}

static char *sprint_range(const struct nf_nat_range2 *r, int family)
{
	bool brackets = family == AF_INET6 &&
			r->flags & NF_NAT_RANGE_PROTO_SPECIFIED;
	static char buf[INET6_ADDRSTRLEN * 2 + 3 + 6 * 3];

	buf[0] = '\0';

	if (r->flags & NF_NAT_RANGE_MAP_IPS) {
		if (brackets)
			strcat(buf, "[");
		inet_ntop(family, &r->min_addr,
			  buf + strlen(buf), INET6_ADDRSTRLEN);
		if (memcmp(&r->min_addr, &r->max_addr, sizeof(r->min_addr))) {
			strcat(buf, "-");
			inet_ntop(family, &r->max_addr,
				  buf + strlen(buf), INET6_ADDRSTRLEN);
		}
		if (brackets)
			strcat(buf, "]");
	}
	if (r->flags & NF_NAT_RANGE_PROTO_SPECIFIED) {
		sprintf(buf + strlen(buf), ":%hu",
			ntohs(r->min_proto.tcp.port));
		if (r->max_proto.tcp.port != r->min_proto.tcp.port)
			sprintf(buf + strlen(buf), "-%hu",
				ntohs(r->max_proto.tcp.port));
		if (r->flags & NF_NAT_RANGE_PROTO_OFFSET)
			sprintf(buf + strlen(buf), "/%hu",
				ntohs(r->base_proto.tcp.port));
	}
	return buf;
}

static void __NAT_print(const struct nf_nat_range2 *r, int family,
			const char *rangeopt, const char *flag_pfx,
			bool skip_colon)
{
	char *range_str = sprint_range(r, family);

	if (strlen(range_str)) {
		if (range_str[0] == ':' && skip_colon)
			range_str++;
		printf(" %s%s", rangeopt, range_str);
	}
	if (r->flags & NF_NAT_RANGE_PROTO_RANDOM)
		printf(" %srandom", flag_pfx);
	if (r->flags & NF_NAT_RANGE_PERSISTENT)
		printf(" %spersistent", flag_pfx);
}
#define __DNAT_print(r, family) __NAT_print(r, family, "to:", "", false)
#define __DNAT_save(r, family) __NAT_print(r, family, "--to-destination ", "--", false)
#define __REDIRECT_print(r) __NAT_print(r, AF_INET, "redir ports ", "", true)
#define __REDIRECT_save(r) __NAT_print(r, AF_INET, "--to-ports ", "--", true)

static void DNAT_print(const void *ip, const struct xt_entry_target *target,
                       int numeric)
{
	struct nf_nat_range2 range = RANGE2_INIT_FROM_IPV4_MRC(target->data);

	__DNAT_print(&range, AF_INET);
}

static void DNAT_save(const void *ip, const struct xt_entry_target *target)
{
	struct nf_nat_range2 range = RANGE2_INIT_FROM_IPV4_MRC(target->data);

	__DNAT_save(&range, AF_INET);
}

static int
__DNAT_xlate(struct xt_xlate *xl, const struct nf_nat_range2 *r, int family)
{
	char *range_str = sprint_range(r, family);
	const char *sep = " ";

	/* shifted portmap ranges are not supported by nftables */
	if (r->flags & NF_NAT_RANGE_PROTO_OFFSET)
		return 0;

	xt_xlate_add(xl, "dnat");
	if (strlen(range_str))
		xt_xlate_add(xl, " to %s", range_str);
	if (r->flags & NF_NAT_RANGE_PROTO_RANDOM) {
		xt_xlate_add(xl, "%srandom", sep);
		sep = ",";
	}
	if (r->flags & NF_NAT_RANGE_PERSISTENT) {
		xt_xlate_add(xl, "%spersistent", sep);
		sep = ",";
	}
	return 1;
}

static int DNAT_xlate(struct xt_xlate *xl,
		      const struct xt_xlate_tg_params *params)
{
	struct nf_nat_range2 range =
		RANGE2_INIT_FROM_IPV4_MRC(params->target->data);

	return __DNAT_xlate(xl, &range, AF_INET);
}

static void DNAT_parse_v2(struct xt_option_call *cb)
{
	const struct ipt_entry *entry = cb->xt_entry;

	__DNAT_parse(cb, entry->ip.proto, cb->data, AF_INET);
}

static void DNAT_fcheck_v2(struct xt_fcheck_call *cb)
{
	__DNAT_fcheck(cb, &((struct nf_nat_range2 *)cb->data)->flags);
}

static void DNAT_print_v2(const void *ip, const struct xt_entry_target *target,
                       int numeric)
{
	__DNAT_print((const void *)target->data, AF_INET);
}

static void DNAT_save_v2(const void *ip, const struct xt_entry_target *target)
{
	__DNAT_save((const void *)target->data, AF_INET);
}

static int DNAT_xlate_v2(struct xt_xlate *xl,
			  const struct xt_xlate_tg_params *params)
{
	return __DNAT_xlate(xl, (const void *)params->target->data, AF_INET);
}

static void DNAT_parse6(struct xt_option_call *cb)
{
	const struct ip6t_entry *entry = cb->xt_entry;
	struct nf_nat_range *range_v1 = (void *)cb->data;
	struct nf_nat_range2 range = {};

	memcpy(&range, range_v1, sizeof(*range_v1));
	__DNAT_parse(cb, entry->ipv6.proto, &range, AF_INET6);
	memcpy(range_v1, &range, sizeof(*range_v1));
}

static void DNAT_fcheck6(struct xt_fcheck_call *cb)
{
	struct nf_nat_range *range = (void *)cb->data;

	if (range->flags & NF_NAT_RANGE_PROTO_OFFSET)
		xtables_error(PARAMETER_PROBLEM,
			      "Shifted portmap ranges not supported with this kernel");

	__DNAT_fcheck(cb, &range->flags);
}

static void DNAT_print6(const void *ip, const struct xt_entry_target *target,
			int numeric)
{
	struct nf_nat_range2 range = {};

	memcpy(&range, (const void *)target->data, sizeof(struct nf_nat_range));
	__DNAT_print(&range, AF_INET6);
}

static void DNAT_save6(const void *ip, const struct xt_entry_target *target)
{
	struct nf_nat_range2 range = {};

	memcpy(&range, (const void *)target->data, sizeof(struct nf_nat_range));
	__DNAT_save(&range, AF_INET6);
}

static int DNAT_xlate6(struct xt_xlate *xl,
		       const struct xt_xlate_tg_params *params)
{
	struct nf_nat_range2 range = {};

	memcpy(&range, (const void *)params->target->data,
	       sizeof(struct nf_nat_range));
	return __DNAT_xlate(xl, &range, AF_INET6);
}

static void DNAT_parse6_v2(struct xt_option_call *cb)
{
	const struct ip6t_entry *entry = cb->xt_entry;

	__DNAT_parse(cb, entry->ipv6.proto, cb->data, AF_INET6);
}

static void DNAT_print6_v2(const void *ip, const struct xt_entry_target *target,
			   int numeric)
{
	__DNAT_print((const void *)target->data, AF_INET6);
}

static void DNAT_save6_v2(const void *ip, const struct xt_entry_target *target)
{
	__DNAT_save((const void *)target->data, AF_INET6);
}

static int DNAT_xlate6_v2(struct xt_xlate *xl,
			  const struct xt_xlate_tg_params *params)
{
	return __DNAT_xlate(xl, (const void *)params->target->data, AF_INET6);
}

static int __REDIRECT_xlate(struct xt_xlate *xl,
			    const struct nf_nat_range2 *range)
{
	char *range_str = sprint_range(range, AF_INET);

	xt_xlate_add(xl, "redirect");
	if (strlen(range_str))
		xt_xlate_add(xl, " to %s", range_str);
	if (range->flags & NF_NAT_RANGE_PROTO_RANDOM)
		xt_xlate_add(xl, " random");

	return 1;
}

static void REDIRECT_print(const void *ip, const struct xt_entry_target *target,
                           int numeric)
{
	struct nf_nat_range2 range = RANGE2_INIT_FROM_IPV4_MRC(target->data);

	__REDIRECT_print(&range);
}

static void REDIRECT_save(const void *ip, const struct xt_entry_target *target)
{
	struct nf_nat_range2 range = RANGE2_INIT_FROM_IPV4_MRC(target->data);

	__REDIRECT_save(&range);
}

static int REDIRECT_xlate(struct xt_xlate *xl,
			   const struct xt_xlate_tg_params *params)
{
	struct nf_nat_range2 range =
		RANGE2_INIT_FROM_IPV4_MRC(params->target->data);

	return __REDIRECT_xlate(xl, &range);
}

static void REDIRECT_print6(const void *ip, const struct xt_entry_target *target,
                            int numeric)
{
	struct nf_nat_range2 range = {};

	memcpy(&range, (const void *)target->data, sizeof(struct nf_nat_range));
	__REDIRECT_print(&range);
}

static void REDIRECT_save6(const void *ip, const struct xt_entry_target *target)
{
	struct nf_nat_range2 range = {};

	memcpy(&range, (const void *)target->data, sizeof(struct nf_nat_range));
	__REDIRECT_save(&range);
}

static int REDIRECT_xlate6(struct xt_xlate *xl,
			   const struct xt_xlate_tg_params *params)
{
	struct nf_nat_range2 range = {};

	memcpy(&range, (const void *)params->target->data,
	       sizeof(struct nf_nat_range));
	return __REDIRECT_xlate(xl, &range);
}

static struct xtables_target dnat_tg_reg[] = {
	{
		.name		= "DNAT",
		.version	= XTABLES_VERSION,
		.family		= NFPROTO_IPV4,
		.revision	= 0,
		.size		= XT_ALIGN(sizeof(struct nf_nat_ipv4_multi_range_compat)),
		.userspacesize	= XT_ALIGN(sizeof(struct nf_nat_ipv4_multi_range_compat)),
		.help		= DNAT_help,
		.print		= DNAT_print,
		.save		= DNAT_save,
		.x6_parse	= DNAT_parse,
		.x6_fcheck	= DNAT_fcheck,
		.x6_options	= DNAT_opts,
		.xlate		= DNAT_xlate,
	},
	{
		.name		= "REDIRECT",
		.version	= XTABLES_VERSION,
		.family		= NFPROTO_IPV4,
		.revision	= 0,
		.size		= XT_ALIGN(sizeof(struct nf_nat_ipv4_multi_range_compat)),
		.userspacesize	= XT_ALIGN(sizeof(struct nf_nat_ipv4_multi_range_compat)),
		.help		= REDIRECT_help,
		.print		= REDIRECT_print,
		.save		= REDIRECT_save,
		.x6_parse	= DNAT_parse,
		.x6_fcheck	= DNAT_fcheck,
		.x6_options	= REDIRECT_opts,
		.xlate		= REDIRECT_xlate,
	},
	{
		.name		= "DNAT",
		.version	= XTABLES_VERSION,
		.family		= NFPROTO_IPV6,
		.revision	= 1,
		.size		= XT_ALIGN(sizeof(struct nf_nat_range)),
		.userspacesize	= XT_ALIGN(sizeof(struct nf_nat_range)),
		.help		= DNAT_help,
		.print		= DNAT_print6,
		.save		= DNAT_save6,
		.x6_parse	= DNAT_parse6,
		.x6_fcheck	= DNAT_fcheck6,
		.x6_options	= DNAT_opts,
		.xlate		= DNAT_xlate6,
	},
	{
		.name		= "REDIRECT",
		.version	= XTABLES_VERSION,
		.family		= NFPROTO_IPV6,
		.size		= XT_ALIGN(sizeof(struct nf_nat_range)),
		.userspacesize	= XT_ALIGN(sizeof(struct nf_nat_range)),
		.help		= REDIRECT_help,
		.print		= REDIRECT_print6,
		.save		= REDIRECT_save6,
		.x6_parse	= DNAT_parse6,
		.x6_fcheck	= DNAT_fcheck6,
		.x6_options	= REDIRECT_opts,
		.xlate		= REDIRECT_xlate6,
	},
	{
		.name		= "DNAT",
		.version	= XTABLES_VERSION,
		.family		= NFPROTO_IPV4,
		.revision	= 2,
		.size		= XT_ALIGN(sizeof(struct nf_nat_range2)),
		.userspacesize	= XT_ALIGN(sizeof(struct nf_nat_range2)),
		.help		= DNAT_help_v2,
		.print		= DNAT_print_v2,
		.save		= DNAT_save_v2,
		.x6_parse	= DNAT_parse_v2,
		.x6_fcheck	= DNAT_fcheck_v2,
		.x6_options	= DNAT_opts,
		.xlate		= DNAT_xlate_v2,
	},
	{
		.name		= "DNAT",
		.version	= XTABLES_VERSION,
		.family		= NFPROTO_IPV6,
		.revision	= 2,
		.size		= XT_ALIGN(sizeof(struct nf_nat_range2)),
		.userspacesize	= XT_ALIGN(sizeof(struct nf_nat_range2)),
		.help		= DNAT_help_v2,
		.print		= DNAT_print6_v2,
		.save		= DNAT_save6_v2,
		.x6_parse	= DNAT_parse6_v2,
		.x6_fcheck	= DNAT_fcheck_v2,
		.x6_options	= DNAT_opts,
		.xlate		= DNAT_xlate6_v2,
	},
};

void _init(void)
{
	xtables_register_targets(dnat_tg_reg, ARRAY_SIZE(dnat_tg_reg));
}
