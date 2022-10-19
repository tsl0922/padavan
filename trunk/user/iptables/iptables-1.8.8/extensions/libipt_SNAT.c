#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <xtables.h>
#include <iptables.h>
#include <limits.h> /* INT_MAX in ip_tables.h */
#include <linux/netfilter_ipv4/ip_tables.h>
#include <linux/netfilter/nf_nat.h>

enum {
	O_TO_SRC = 0,
	O_RANDOM,
	O_RANDOM_FULLY,
	O_PERSISTENT,
	F_TO_SRC       = 1 << O_TO_SRC,
	F_RANDOM       = 1 << O_RANDOM,
	F_RANDOM_FULLY = 1 << O_RANDOM_FULLY,
};

static void SNAT_help(void)
{
	printf(
"SNAT target options:\n"
" --to-source [<ipaddr>[-<ipaddr>]][:port[-port]]\n"
"				Address to map source to.\n"
"[--random] [--random-fully] [--persistent]\n");
}

static const struct xt_option_entry SNAT_opts[] = {
	{.name = "to-source", .id = O_TO_SRC, .type = XTTYPE_STRING,
	 .flags = XTOPT_MAND},
	{.name = "random", .id = O_RANDOM, .type = XTTYPE_NONE},
	{.name = "random-fully", .id = O_RANDOM_FULLY, .type = XTTYPE_NONE},
	{.name = "persistent", .id = O_PERSISTENT, .type = XTTYPE_NONE},
	XTOPT_TABLEEND,
};

/* Ranges expected in network order. */
static void
parse_to(const char *orig_arg, int portok, struct nf_nat_ipv4_range *range)
{
	char *arg, *colon, *dash, *error;
	const struct in_addr *ip;

	arg = xtables_strdup(orig_arg);
	colon = strchr(arg, ':');

	if (colon) {
		int port;

		if (!portok)
			xtables_error(PARAMETER_PROBLEM,
				   "Need TCP, UDP, SCTP or DCCP with port specification");

		range->flags |= NF_NAT_RANGE_PROTO_SPECIFIED;

		port = atoi(colon+1);
		if (port <= 0 || port > 65535)
			xtables_error(PARAMETER_PROBLEM,
				   "Port `%s' not valid\n", colon+1);

		error = strchr(colon+1, ':');
		if (error)
			xtables_error(PARAMETER_PROBLEM,
				   "Invalid port:port syntax - use dash\n");

		dash = strchr(colon, '-');
		if (!dash) {
			range->min.tcp.port
				= range->max.tcp.port
				= htons(port);
		} else {
			int maxport;

			maxport = atoi(dash + 1);
			if (maxport <= 0 || maxport > 65535)
				xtables_error(PARAMETER_PROBLEM,
					   "Port `%s' not valid\n", dash+1);
			if (maxport < port)
				/* People are stupid. */
				xtables_error(PARAMETER_PROBLEM,
					   "Port range `%s' funky\n", colon+1);
			range->min.tcp.port = htons(port);
			range->max.tcp.port = htons(maxport);
		}
		/* Starts with a colon? No IP info...*/
		if (colon == arg) {
			free(arg);
			return;
		}
		*colon = '\0';
	}

	range->flags |= NF_NAT_RANGE_MAP_IPS;
	dash = strchr(arg, '-');
	if (colon && dash && dash > colon)
		dash = NULL;

	if (dash)
		*dash = '\0';

	ip = xtables_numeric_to_ipaddr(arg);
	if (!ip)
		xtables_error(PARAMETER_PROBLEM, "Bad IP address \"%s\"\n",
			   arg);
	range->min_ip = ip->s_addr;
	if (dash) {
		ip = xtables_numeric_to_ipaddr(dash+1);
		if (!ip)
			xtables_error(PARAMETER_PROBLEM, "Bad IP address \"%s\"\n",
				   dash+1);
		range->max_ip = ip->s_addr;
	} else
		range->max_ip = range->min_ip;

	free(arg);
	return;
}

static void SNAT_parse(struct xt_option_call *cb)
{
	struct nf_nat_ipv4_multi_range_compat *mr = cb->data;
	const struct ipt_entry *entry = cb->xt_entry;
	int portok;

	if (entry->ip.proto == IPPROTO_TCP
	    || entry->ip.proto == IPPROTO_UDP
	    || entry->ip.proto == IPPROTO_SCTP
	    || entry->ip.proto == IPPROTO_DCCP
	    || entry->ip.proto == IPPROTO_ICMP)
		portok = 1;
	else
		portok = 0;

	xtables_option_parse(cb);
	switch (cb->entry->id) {
	case O_TO_SRC:
		parse_to(cb->arg, portok, mr->range);
		break;
	case O_PERSISTENT:
		mr->range->flags |= NF_NAT_RANGE_PERSISTENT;
		break;
	}
}

static void SNAT_fcheck(struct xt_fcheck_call *cb)
{
	static const unsigned int f = F_TO_SRC | F_RANDOM;
	static const unsigned int r = F_TO_SRC | F_RANDOM_FULLY;
	struct nf_nat_ipv4_multi_range_compat *mr = cb->data;

	if ((cb->xflags & f) == f)
		mr->range->flags |= NF_NAT_RANGE_PROTO_RANDOM;
	if ((cb->xflags & r) == r)
		mr->range->flags |= NF_NAT_RANGE_PROTO_RANDOM_FULLY;

	mr->rangesize = 1;
}

static void print_range(const struct nf_nat_ipv4_range *r)
{
	if (r->flags & NF_NAT_RANGE_MAP_IPS) {
		struct in_addr a;

		a.s_addr = r->min_ip;
		printf("%s", xtables_ipaddr_to_numeric(&a));
		if (r->max_ip != r->min_ip) {
			a.s_addr = r->max_ip;
			printf("-%s", xtables_ipaddr_to_numeric(&a));
		}
	}
	if (r->flags & NF_NAT_RANGE_PROTO_SPECIFIED) {
		printf(":");
		printf("%hu", ntohs(r->min.tcp.port));
		if (r->max.tcp.port != r->min.tcp.port)
			printf("-%hu", ntohs(r->max.tcp.port));
	}
}

static void SNAT_print(const void *ip, const struct xt_entry_target *target,
                       int numeric)
{
	const struct nf_nat_ipv4_multi_range_compat *mr =
				(const void *)target->data;

	printf(" to:");
	print_range(mr->range);
	if (mr->range->flags & NF_NAT_RANGE_PROTO_RANDOM)
		printf(" random");
	if (mr->range->flags & NF_NAT_RANGE_PROTO_RANDOM_FULLY)
		printf(" random-fully");
	if (mr->range->flags & NF_NAT_RANGE_PERSISTENT)
		printf(" persistent");
}

static void SNAT_save(const void *ip, const struct xt_entry_target *target)
{
	const struct nf_nat_ipv4_multi_range_compat *mr =
				(const void *)target->data;

	printf(" --to-source ");
	print_range(mr->range);
	if (mr->range->flags & NF_NAT_RANGE_PROTO_RANDOM)
		printf(" --random");
	if (mr->range->flags & NF_NAT_RANGE_PROTO_RANDOM_FULLY)
		printf(" --random-fully");
	if (mr->range->flags & NF_NAT_RANGE_PERSISTENT)
		printf(" --persistent");
}

static void print_range_xlate(const struct nf_nat_ipv4_range *r,
			      struct xt_xlate *xl)
{
	if (r->flags & NF_NAT_RANGE_MAP_IPS) {
		struct in_addr a;

		a.s_addr = r->min_ip;
		xt_xlate_add(xl, "%s", xtables_ipaddr_to_numeric(&a));
		if (r->max_ip != r->min_ip) {
			a.s_addr = r->max_ip;
			xt_xlate_add(xl, "-%s", xtables_ipaddr_to_numeric(&a));
		}
	}
	if (r->flags & NF_NAT_RANGE_PROTO_SPECIFIED) {
		xt_xlate_add(xl, ":");
		xt_xlate_add(xl, "%hu", ntohs(r->min.tcp.port));
		if (r->max.tcp.port != r->min.tcp.port)
			xt_xlate_add(xl, "-%hu", ntohs(r->max.tcp.port));
	}
}

static int SNAT_xlate(struct xt_xlate *xl,
		      const struct xt_xlate_tg_params *params)
{
	const struct nf_nat_ipv4_multi_range_compat *mr =
				(const void *)params->target->data;
	bool sep_need = false;
	const char *sep = " ";

	xt_xlate_add(xl, "snat to ");
	print_range_xlate(mr->range, xl);
	if (mr->range->flags & NF_NAT_RANGE_PROTO_RANDOM) {
		xt_xlate_add(xl, " random");
		sep_need = true;
	}
	if (mr->range->flags & NF_NAT_RANGE_PROTO_RANDOM_FULLY) {
		if (sep_need)
			sep = ",";
		xt_xlate_add(xl, "%sfully-random", sep);
		sep_need = true;
	}
	if (mr->range->flags & NF_NAT_RANGE_PERSISTENT) {
		if (sep_need)
			sep = ",";
		xt_xlate_add(xl, "%spersistent", sep);
	}

	return 1;
}

static struct xtables_target snat_tg_reg = {
	.name		= "SNAT",
	.version	= XTABLES_VERSION,
	.family		= NFPROTO_IPV4,
	.size		= XT_ALIGN(sizeof(struct nf_nat_ipv4_multi_range_compat)),
	.userspacesize	= XT_ALIGN(sizeof(struct nf_nat_ipv4_multi_range_compat)),
	.help		= SNAT_help,
	.x6_parse	= SNAT_parse,
	.x6_fcheck	= SNAT_fcheck,
	.print		= SNAT_print,
	.save		= SNAT_save,
	.x6_options	= SNAT_opts,
	.xlate		= SNAT_xlate,
};

void _init(void)
{
	xtables_register_target(&snat_tg_reg);
}
