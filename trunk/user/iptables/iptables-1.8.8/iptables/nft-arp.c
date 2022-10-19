/*
 * (C) 2013 by Pablo Neira Ayuso <pablo@netfilter.org>
 * (C) 2013 by Giuseppe Longo <giuseppelng@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This code has been sponsored by Sophos Astaro <http://www.sophos.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <net/if_arp.h>

#include <xtables.h>
#include <libiptc/libxtc.h>
#include <net/if_arp.h>
#include <netinet/if_ether.h>

#include <linux/netfilter_arp/arp_tables.h>
#include <linux/netfilter/nf_tables.h>

#include "nft-shared.h"
#include "nft.h"
#include "xshared.h"

static bool need_devaddr(struct arpt_devaddr_info *info)
{
	int i;

	for (i = 0; i < ETH_ALEN; i++) {
		if (info->addr[i] || info->mask[i])
			return true;
	}

	return false;
}

static int nft_arp_add(struct nft_handle *h, struct nftnl_rule *r,
		       struct iptables_command_state *cs)
{
	struct arpt_entry *fw = &cs->arp;
	uint32_t op;
	int ret = 0;

	if (fw->arp.iniface[0] != '\0') {
		op = nft_invflags2cmp(fw->arp.invflags, IPT_INV_VIA_IN);
		add_iniface(h, r, fw->arp.iniface, op);
	}

	if (fw->arp.outiface[0] != '\0') {
		op = nft_invflags2cmp(fw->arp.invflags, IPT_INV_VIA_OUT);
		add_outiface(h, r, fw->arp.outiface, op);
	}

	if (fw->arp.arhrd != 0 ||
	    fw->arp.invflags & IPT_INV_ARPHRD) {
		uint8_t reg;

		op = nft_invflags2cmp(fw->arp.invflags, IPT_INV_ARPHRD);
		add_payload(h, r, offsetof(struct arphdr, ar_hrd), 2,
			    NFT_PAYLOAD_NETWORK_HEADER, &reg);
		add_cmp_u16(r, fw->arp.arhrd, op, reg);
	}

	if (fw->arp.arpro != 0 ||
	    fw->arp.invflags & IPT_INV_PROTO) {
		uint8_t reg;

		op = nft_invflags2cmp(fw->arp.invflags, IPT_INV_PROTO);
	        add_payload(h, r, offsetof(struct arphdr, ar_pro), 2,
			    NFT_PAYLOAD_NETWORK_HEADER, &reg);
		add_cmp_u16(r, fw->arp.arpro, op, reg);
	}

	if (fw->arp.arhln != 0 ||
	    fw->arp.invflags & IPT_INV_ARPHLN) {
		op = nft_invflags2cmp(fw->arp.invflags, IPT_INV_ARPHLN);
		add_proto(h, r, offsetof(struct arphdr, ar_hln), 1,
			  fw->arp.arhln, op);
	}

	add_proto(h, r, offsetof(struct arphdr, ar_pln), 1, 4, NFT_CMP_EQ);

	if (fw->arp.arpop != 0 ||
	    fw->arp.invflags & IPT_INV_ARPOP) {
		uint8_t reg;

		op = nft_invflags2cmp(fw->arp.invflags, IPT_INV_ARPOP);
		add_payload(h, r, offsetof(struct arphdr, ar_op), 2,
			    NFT_PAYLOAD_NETWORK_HEADER, &reg);
		add_cmp_u16(r, fw->arp.arpop, op, reg);
	}

	if (need_devaddr(&fw->arp.src_devaddr)) {
		op = nft_invflags2cmp(fw->arp.invflags, IPT_INV_SRCDEVADDR);
		add_addr(h, r, NFT_PAYLOAD_NETWORK_HEADER,
			 sizeof(struct arphdr),
			 &fw->arp.src_devaddr.addr,
			 &fw->arp.src_devaddr.mask,
			 fw->arp.arhln, op);

	}

	if (fw->arp.src.s_addr != 0 ||
	    fw->arp.smsk.s_addr != 0 ||
	    fw->arp.invflags & IPT_INV_SRCIP) {
		op = nft_invflags2cmp(fw->arp.invflags, IPT_INV_SRCIP);
		add_addr(h, r, NFT_PAYLOAD_NETWORK_HEADER,
			 sizeof(struct arphdr) + fw->arp.arhln,
			 &fw->arp.src.s_addr, &fw->arp.smsk.s_addr,
			 sizeof(struct in_addr), op);
	}


	if (need_devaddr(&fw->arp.tgt_devaddr)) {
		op = nft_invflags2cmp(fw->arp.invflags, IPT_INV_TGTDEVADDR);
		add_addr(h, r, NFT_PAYLOAD_NETWORK_HEADER,
			 sizeof(struct arphdr) + fw->arp.arhln + sizeof(struct in_addr),
			 &fw->arp.tgt_devaddr.addr,
			 &fw->arp.tgt_devaddr.mask,
			 fw->arp.arhln, op);
	}

	if (fw->arp.tgt.s_addr != 0 ||
	    fw->arp.tmsk.s_addr != 0 ||
	    fw->arp.invflags & IPT_INV_DSTIP) {
		op = nft_invflags2cmp(fw->arp.invflags, IPT_INV_DSTIP);
		add_addr(h, r, NFT_PAYLOAD_NETWORK_HEADER,
			 sizeof(struct arphdr) + fw->arp.arhln + sizeof(struct in_addr) + fw->arp.arhln,
			 &fw->arp.tgt.s_addr, &fw->arp.tmsk.s_addr,
			 sizeof(struct in_addr), op);
	}

	/* Counters need to me added before the target, otherwise they are
	 * increased for each rule because of the way nf_tables works.
	 */
	if (add_counters(r, fw->counters.pcnt, fw->counters.bcnt) < 0)
		return -1;

	if (cs->target != NULL) {
		/* Standard target? */
		if (strcmp(cs->jumpto, XTC_LABEL_ACCEPT) == 0)
			ret = add_verdict(r, NF_ACCEPT);
		else if (strcmp(cs->jumpto, XTC_LABEL_DROP) == 0)
			ret = add_verdict(r, NF_DROP);
		else if (strcmp(cs->jumpto, XTC_LABEL_RETURN) == 0)
			ret = add_verdict(r, NFT_RETURN);
		else
			ret = add_target(r, cs->target->t);
	} else if (strlen(cs->jumpto) > 0) {
		/* No goto in arptables */
		ret = add_jumpto(r, cs->jumpto, NFT_JUMP);
	}

	return ret;
}

static void nft_arp_parse_meta(struct nft_xt_ctx *ctx, struct nftnl_expr *e,
			       struct iptables_command_state *cs)
{
	struct arpt_entry *fw = &cs->arp;
	uint8_t flags = 0;

	parse_meta(ctx, e, ctx->meta.key, fw->arp.iniface, fw->arp.iniface_mask,
		   fw->arp.outiface, fw->arp.outiface_mask,
		   &flags);

	fw->arp.invflags |= flags;
}

static void parse_mask_ipv4(struct nft_xt_ctx *ctx, struct in_addr *mask)
{
	mask->s_addr = ctx->bitwise.mask[0];
}

static bool nft_arp_parse_devaddr(struct nft_xt_ctx *ctx,
				  struct nftnl_expr *e,
				  struct arpt_devaddr_info *info)
{
	uint32_t hlen;
	bool inv;

	nftnl_expr_get(e, NFTNL_EXPR_CMP_DATA, &hlen);

	if (hlen != ETH_ALEN)
		return false;

	get_cmp_data(e, info->addr, ETH_ALEN, &inv);

	if (ctx->flags & NFT_XT_CTX_BITWISE) {
		memcpy(info->mask, ctx->bitwise.mask, ETH_ALEN);
		ctx->flags &= ~NFT_XT_CTX_BITWISE;
	} else {
		memset(info->mask, 0xff,
		       min(ctx->payload.len, ETH_ALEN));
	}

	return inv;
}

static void nft_arp_parse_payload(struct nft_xt_ctx *ctx,
				  struct nftnl_expr *e,
				  struct iptables_command_state *cs)
{
	struct arpt_entry *fw = &cs->arp;
	struct in_addr addr;
	uint16_t ar_hrd, ar_pro, ar_op;
	uint8_t ar_hln;
	bool inv;

	switch (ctx->payload.offset) {
	case offsetof(struct arphdr, ar_hrd):
		get_cmp_data(e, &ar_hrd, sizeof(ar_hrd), &inv);
		fw->arp.arhrd = ar_hrd;
		fw->arp.arhrd_mask = 0xffff;
		if (inv)
			fw->arp.invflags |= IPT_INV_ARPHRD;
		break;
	case offsetof(struct arphdr, ar_pro):
		get_cmp_data(e, &ar_pro, sizeof(ar_pro), &inv);
		fw->arp.arpro = ar_pro;
		fw->arp.arpro_mask = 0xffff;
		if (inv)
			fw->arp.invflags |= IPT_INV_PROTO;
		break;
	case offsetof(struct arphdr, ar_op):
		get_cmp_data(e, &ar_op, sizeof(ar_op), &inv);
		fw->arp.arpop = ar_op;
		fw->arp.arpop_mask = 0xffff;
		if (inv)
			fw->arp.invflags |= IPT_INV_ARPOP;
		break;
	case offsetof(struct arphdr, ar_hln):
		get_cmp_data(e, &ar_hln, sizeof(ar_hln), &inv);
		fw->arp.arhln = ar_hln;
		fw->arp.arhln_mask = 0xff;
		if (inv)
			fw->arp.invflags |= IPT_INV_ARPOP;
		break;
	default:
		if (ctx->payload.offset == sizeof(struct arphdr)) {
			if (nft_arp_parse_devaddr(ctx, e, &fw->arp.src_devaddr))
				fw->arp.invflags |= IPT_INV_SRCDEVADDR;
		} else if (ctx->payload.offset == sizeof(struct arphdr) +
					   fw->arp.arhln) {
			get_cmp_data(e, &addr, sizeof(addr), &inv);
			fw->arp.src.s_addr = addr.s_addr;
			if (ctx->flags & NFT_XT_CTX_BITWISE) {
				parse_mask_ipv4(ctx, &fw->arp.smsk);
				ctx->flags &= ~NFT_XT_CTX_BITWISE;
			} else {
				memset(&fw->arp.smsk, 0xff,
				       min(ctx->payload.len,
					   sizeof(struct in_addr)));
			}

			if (inv)
				fw->arp.invflags |= IPT_INV_SRCIP;
		} else if (ctx->payload.offset == sizeof(struct arphdr) +
						  fw->arp.arhln +
						  sizeof(struct in_addr)) {
			if (nft_arp_parse_devaddr(ctx, e, &fw->arp.tgt_devaddr))
				fw->arp.invflags |= IPT_INV_TGTDEVADDR;
		} else if (ctx->payload.offset == sizeof(struct arphdr) +
						  fw->arp.arhln +
						  sizeof(struct in_addr) +
						  fw->arp.arhln) {
			get_cmp_data(e, &addr, sizeof(addr), &inv);
			fw->arp.tgt.s_addr = addr.s_addr;
			if (ctx->flags & NFT_XT_CTX_BITWISE) {
				parse_mask_ipv4(ctx, &fw->arp.tmsk);
				ctx->flags &= ~NFT_XT_CTX_BITWISE;
			} else {
				memset(&fw->arp.tmsk, 0xff,
				       min(ctx->payload.len,
					   sizeof(struct in_addr)));
			}

			if (inv)
				fw->arp.invflags |= IPT_INV_DSTIP;
		}
		break;
	}
}

static void nft_arp_print_header(unsigned int format, const char *chain,
				 const char *pol,
				 const struct xt_counters *counters,
				 int refs, uint32_t entries)
{
	printf("Chain %s", chain);
	if (pol) {
		printf(" (policy %s", pol);
		if (!(format & FMT_NOCOUNTS)) {
			fputc(' ', stdout);
			xtables_print_num(counters->pcnt, (format|FMT_NOTABLE));
			fputs("packets, ", stdout);
			xtables_print_num(counters->bcnt, (format|FMT_NOTABLE));
			fputs("bytes", stdout);
		}
		printf(")\n");
	} else {
		printf(" (%d references)\n", refs);
	}
}

static void nft_arp_print_rule_details(const struct iptables_command_state *cs,
				       unsigned int format)
{
	const struct arpt_entry *fw = &cs->arp;
	char iface[IFNAMSIZ+2];
	const char *sep = "";
	int print_iface = 0;
	int i;

	if (strlen(cs->jumpto)) {
		printf("%s-j %s", sep, cs->jumpto);
		sep = " ";
	}

	iface[0] = '\0';

	if (fw->arp.iniface[0] != '\0') {
		strcat(iface, fw->arp.iniface);
		print_iface = 1;
	}
	else if (format & FMT_VIA) {
		print_iface = 1;
		if (format & FMT_NUMERIC) strcat(iface, "*");
		else strcat(iface, "any");
	}
	if (print_iface) {
		printf("%s%s-i %s", sep, fw->arp.invflags & IPT_INV_VIA_IN ?
				   "! " : "", iface);
		sep = " ";
	}

	print_iface = 0;
	iface[0] = '\0';

	if (fw->arp.outiface[0] != '\0') {
		strcat(iface, fw->arp.outiface);
		print_iface = 1;
	}
	else if (format & FMT_VIA) {
		print_iface = 1;
		if (format & FMT_NUMERIC) strcat(iface, "*");
		else strcat(iface, "any");
	}
	if (print_iface) {
		printf("%s%s-o %s", sep, fw->arp.invflags & IPT_INV_VIA_OUT ?
				   "! " : "", iface);
		sep = " ";
	}

	if (fw->arp.smsk.s_addr != 0L) {
		printf("%s%s-s %s", sep,
		       fw->arp.invflags & IPT_INV_SRCIP ? "! " : "",
		       ipv4_addr_to_string(&fw->arp.src,
					   &fw->arp.smsk, format));
		sep = " ";
	}

	for (i = 0; i < ARPT_DEV_ADDR_LEN_MAX; i++)
		if (fw->arp.src_devaddr.mask[i] != 0)
			break;
	if (i == ARPT_DEV_ADDR_LEN_MAX)
		goto after_devsrc;
	printf("%s%s", sep, fw->arp.invflags & IPT_INV_SRCDEVADDR
		? "! " : "");
	printf("--src-mac ");
	xtables_print_mac_and_mask((unsigned char *)fw->arp.src_devaddr.addr,
				   (unsigned char *)fw->arp.src_devaddr.mask);
	sep = " ";
after_devsrc:

	if (fw->arp.tmsk.s_addr != 0L) {
		printf("%s%s-d %s", sep,
		       fw->arp.invflags & IPT_INV_DSTIP ? "! " : "",
		       ipv4_addr_to_string(&fw->arp.tgt,
					   &fw->arp.tmsk, format));
		sep = " ";
	}

	for (i = 0; i <ARPT_DEV_ADDR_LEN_MAX; i++)
		if (fw->arp.tgt_devaddr.mask[i] != 0)
			break;
	if (i == ARPT_DEV_ADDR_LEN_MAX)
		goto after_devdst;
	printf("%s%s", sep, fw->arp.invflags & IPT_INV_TGTDEVADDR
		? "! " : "");
	printf("--dst-mac ");
	xtables_print_mac_and_mask((unsigned char *)fw->arp.tgt_devaddr.addr,
				   (unsigned char *)fw->arp.tgt_devaddr.mask);
	sep = " ";

after_devdst:

	if (fw->arp.arhln_mask != 255 || fw->arp.arhln != 6) {
		printf("%s%s", sep, fw->arp.invflags & IPT_INV_ARPHLN
			? "! " : "");
		printf("--h-length %d", fw->arp.arhln);
		if (fw->arp.arhln_mask != 255)
			printf("/%d", fw->arp.arhln_mask);
		sep = " ";
	}

	if (fw->arp.arpop_mask != 0) {
		int tmp = ntohs(fw->arp.arpop);

		printf("%s%s", sep, fw->arp.invflags & IPT_INV_ARPOP
			? "! " : "");
		if (tmp <= ARP_NUMOPCODES && !(format & FMT_NUMERIC))
			printf("--opcode %s", arp_opcodes[tmp-1]);
		else
			printf("--opcode %d", tmp);

		if (fw->arp.arpop_mask != 65535)
			printf("/%d", ntohs(fw->arp.arpop_mask));
		sep = " ";
	}

	if (fw->arp.arhrd_mask != 65535 || fw->arp.arhrd != htons(1)) {
		uint16_t tmp = ntohs(fw->arp.arhrd);

		printf("%s%s", sep, fw->arp.invflags & IPT_INV_ARPHRD
			? "! " : "");
		if (tmp == 1 && !(format & FMT_NUMERIC))
			printf("--h-type %s", "Ethernet");
		else
			printf("--h-type %u", tmp);
		if (fw->arp.arhrd_mask != 65535)
			printf("/%d", ntohs(fw->arp.arhrd_mask));
		sep = " ";
	}

	if (fw->arp.arpro_mask != 0) {
		int tmp = ntohs(fw->arp.arpro);

		printf("%s%s", sep, fw->arp.invflags & IPT_INV_PROTO
			? "! " : "");
		if (tmp == 0x0800 && !(format & FMT_NUMERIC))
			printf("--proto-type %s", "IPv4");
		else
			printf("--proto-type 0x%x", tmp);
		if (fw->arp.arpro_mask != 65535)
			printf("/%x", ntohs(fw->arp.arpro_mask));
		sep = " ";
	}
}

static void
nft_arp_save_rule(const struct iptables_command_state *cs, unsigned int format)
{
	format |= FMT_NUMERIC;

	printf(" ");
	nft_arp_print_rule_details(cs, format);
	if (cs->target && cs->target->save)
		cs->target->save(&cs->fw, cs->target->t);
	printf("\n");
}

static void
nft_arp_print_rule(struct nft_handle *h, struct nftnl_rule *r,
		   unsigned int num, unsigned int format)
{
	struct iptables_command_state cs = {};

	if (format & FMT_LINENUMBERS)
		printf("%u ", num);

	nft_rule_to_iptables_command_state(h, r, &cs);

	nft_arp_print_rule_details(&cs, format);
	print_matches_and_target(&cs, format);

	if (!(format & FMT_NOCOUNTS)) {
		printf(" , pcnt=");
		xtables_print_num(cs.counters.pcnt, format | FMT_NOTABLE);
		printf("-- bcnt=");
		xtables_print_num(cs.counters.bcnt, format | FMT_NOTABLE);
	}

	if (!(format & FMT_NONEWLINE))
		fputc('\n', stdout);

	nft_clear_iptables_command_state(&cs);
}

static bool nft_arp_is_same(const struct iptables_command_state *cs_a,
			    const struct iptables_command_state *cs_b)
{
	const struct arpt_entry *a = &cs_a->arp;
	const struct arpt_entry *b = &cs_b->arp;

	if (a->arp.src.s_addr != b->arp.src.s_addr
	    || a->arp.tgt.s_addr != b->arp.tgt.s_addr
	    || a->arp.smsk.s_addr != b->arp.smsk.s_addr
	    || a->arp.tmsk.s_addr != b->arp.tmsk.s_addr
	    || a->arp.arpro != b->arp.arpro
	    || a->arp.flags != b->arp.flags
	    || a->arp.invflags != b->arp.invflags) {
		DEBUGP("different src/dst/proto/flags/invflags\n");
		return false;
	}

	return is_same_interfaces(a->arp.iniface,
				  a->arp.outiface,
				  (unsigned char *)a->arp.iniface_mask,
				  (unsigned char *)a->arp.outiface_mask,
				  b->arp.iniface,
				  b->arp.outiface,
				  (unsigned char *)b->arp.iniface_mask,
				  (unsigned char *)b->arp.outiface_mask);
}

static void nft_arp_save_chain(const struct nftnl_chain *c, const char *policy)
{
	const char *chain = nftnl_chain_get_str(c, NFTNL_CHAIN_NAME);

	printf(":%s %s\n", chain, policy ?: "-");
}

static int getlength_and_mask(const char *from, uint8_t *to, uint8_t *mask)
{
	char *dup = strdup(from);
	char *p, *buffer;
	int i, ret = -1;

	if (!dup)
		return -1;

	if ( (p = strrchr(dup, '/')) != NULL) {
		*p = '\0';
		i = strtol(p+1, &buffer, 10);
		if (*buffer != '\0' || i < 0 || i > 255)
			goto out_err;
		*mask = (uint8_t)i;
	} else
		*mask = 255;
	i = strtol(dup, &buffer, 10);
	if (*buffer != '\0' || i < 0 || i > 255)
		goto out_err;
	*to = (uint8_t)i;
	ret = 0;
out_err:
	free(dup);
	return ret;

}

static int get16_and_mask(const char *from, uint16_t *to,
			  uint16_t *mask, int base)
{
	char *dup = strdup(from);
	char *p, *buffer;
	int i, ret = -1;

	if (!dup)
		return -1;

	if ( (p = strrchr(dup, '/')) != NULL) {
		*p = '\0';
		i = strtol(p+1, &buffer, base);
		if (*buffer != '\0' || i < 0 || i > 65535)
			goto out_err;
		*mask = htons((uint16_t)i);
	} else
		*mask = 65535;
	i = strtol(dup, &buffer, base);
	if (*buffer != '\0' || i < 0 || i > 65535)
		goto out_err;
	*to = htons((uint16_t)i);
	ret = 0;
out_err:
	free(dup);
	return ret;
}

static void nft_arp_post_parse(int command,
			       struct iptables_command_state *cs,
			       struct xtables_args *args)
{
	cs->arp.arp.invflags = args->invflags;

	memcpy(cs->arp.arp.iniface, args->iniface, IFNAMSIZ);
	memcpy(cs->arp.arp.iniface_mask, args->iniface_mask, IFNAMSIZ);

	memcpy(cs->arp.arp.outiface, args->outiface, IFNAMSIZ);
	memcpy(cs->arp.arp.outiface_mask, args->outiface_mask, IFNAMSIZ);

	cs->arp.counters.pcnt = args->pcnt_cnt;
	cs->arp.counters.bcnt = args->bcnt_cnt;

	if (command & (CMD_REPLACE | CMD_INSERT |
			CMD_DELETE | CMD_APPEND | CMD_CHECK)) {
		if (!(cs->options & OPT_DESTINATION))
			args->dhostnetworkmask = "0.0.0.0/0";
		if (!(cs->options & OPT_SOURCE))
			args->shostnetworkmask = "0.0.0.0/0";
	}

	if (args->shostnetworkmask)
		xtables_ipparse_multiple(args->shostnetworkmask,
					 &args->s.addr.v4, &args->s.mask.v4,
					 &args->s.naddrs);
	if (args->dhostnetworkmask)
		xtables_ipparse_multiple(args->dhostnetworkmask,
					 &args->d.addr.v4, &args->d.mask.v4,
					 &args->d.naddrs);

	if ((args->s.naddrs > 1 || args->d.naddrs > 1) &&
	    (cs->arp.arp.invflags & (ARPT_INV_SRCIP | ARPT_INV_TGTIP)))
		xtables_error(PARAMETER_PROBLEM,
			      "! not allowed with multiple"
			      " source or destination IP addresses");

	if (args->src_mac &&
	    xtables_parse_mac_and_mask(args->src_mac,
				       cs->arp.arp.src_devaddr.addr,
				       cs->arp.arp.src_devaddr.mask))
		xtables_error(PARAMETER_PROBLEM,
			      "Problem with specified source mac");
	if (args->dst_mac &&
	    xtables_parse_mac_and_mask(args->dst_mac,
				       cs->arp.arp.tgt_devaddr.addr,
				       cs->arp.arp.tgt_devaddr.mask))
		xtables_error(PARAMETER_PROBLEM,
			      "Problem with specified destination mac");
	if (args->arp_hlen) {
		getlength_and_mask(args->arp_hlen, &cs->arp.arp.arhln,
				   &cs->arp.arp.arhln_mask);

		if (cs->arp.arp.arhln != 6)
			xtables_error(PARAMETER_PROBLEM,
				      "Only harware address length of 6 is supported currently.");
	}
	if (args->arp_opcode) {
		if (get16_and_mask(args->arp_opcode, &cs->arp.arp.arpop,
				   &cs->arp.arp.arpop_mask, 10)) {
			int i;

			for (i = 0; i < ARP_NUMOPCODES; i++)
				if (!strcasecmp(arp_opcodes[i],
						args->arp_opcode))
					break;
			if (i == ARP_NUMOPCODES)
				xtables_error(PARAMETER_PROBLEM,
					      "Problem with specified opcode");
			cs->arp.arp.arpop = htons(i+1);
		}
	}
	if (args->arp_htype) {
		if (get16_and_mask(args->arp_htype, &cs->arp.arp.arhrd,
				   &cs->arp.arp.arhrd_mask, 16)) {
			if (strcasecmp(args->arp_htype, "Ethernet"))
				xtables_error(PARAMETER_PROBLEM,
					      "Problem with specified hardware type");
			cs->arp.arp.arhrd = htons(1);
		}
	}
	if (args->arp_ptype) {
		if (get16_and_mask(args->arp_ptype, &cs->arp.arp.arpro,
				   &cs->arp.arp.arpro_mask, 0)) {
			if (strcasecmp(args->arp_ptype, "ipv4"))
				xtables_error(PARAMETER_PROBLEM,
					      "Problem with specified protocol type");
			cs->arp.arp.arpro = htons(0x800);
		}
	}
}

static void nft_arp_init_cs(struct iptables_command_state *cs)
{
	cs->arp.arp.arhln = 6;
	cs->arp.arp.arhln_mask = 255;
	cs->arp.arp.arhrd = htons(ARPHRD_ETHER);
	cs->arp.arp.arhrd_mask = 65535;
}

static int
nft_arp_add_entry(struct nft_handle *h,
		  const char *chain, const char *table,
		  struct iptables_command_state *cs,
		  struct xtables_args *args, bool verbose,
		  bool append, int rulenum)
{
	unsigned int i, j;
	int ret = 1;

	for (i = 0; i < args->s.naddrs; i++) {
		cs->arp.arp.src.s_addr = args->s.addr.v4[i].s_addr;
		cs->arp.arp.smsk.s_addr = args->s.mask.v4[i].s_addr;
		for (j = 0; j < args->d.naddrs; j++) {
			cs->arp.arp.tgt.s_addr = args->d.addr.v4[j].s_addr;
			cs->arp.arp.tmsk.s_addr = args->d.mask.v4[j].s_addr;
			if (append) {
				ret = nft_cmd_rule_append(h, chain, table, cs, NULL,
						          verbose);
			} else {
				ret = nft_cmd_rule_insert(h, chain, table, cs,
						          rulenum, verbose);
			}
		}
	}

	return ret;
}

static int
nft_arp_delete_entry(struct nft_handle *h,
		     const char *chain, const char *table,
		     struct iptables_command_state *cs,
		     struct xtables_args *args, bool verbose)
{
	unsigned int i, j;
	int ret = 1;

	for (i = 0; i < args->s.naddrs; i++) {
		cs->arp.arp.src.s_addr = args->s.addr.v4[i].s_addr;
		cs->arp.arp.smsk.s_addr = args->s.mask.v4[i].s_addr;
		for (j = 0; j < args->d.naddrs; j++) {
			cs->arp.arp.tgt.s_addr = args->d.addr.v4[j].s_addr;
			cs->arp.arp.tmsk.s_addr = args->d.mask.v4[j].s_addr;
			ret = nft_cmd_rule_delete(h, chain, table, cs, verbose);
		}
	}

	return ret;
}

static int
nft_arp_check_entry(struct nft_handle *h,
		    const char *chain, const char *table,
		    struct iptables_command_state *cs,
		    struct xtables_args *args, bool verbose)
{
	unsigned int i, j;
	int ret = 1;

	for (i = 0; i < args->s.naddrs; i++) {
		cs->arp.arp.src.s_addr = args->s.addr.v4[i].s_addr;
		cs->arp.arp.smsk.s_addr = args->s.mask.v4[i].s_addr;
		for (j = 0; j < args->d.naddrs; j++) {
			cs->arp.arp.tgt.s_addr = args->d.addr.v4[j].s_addr;
			cs->arp.arp.tmsk.s_addr = args->d.mask.v4[j].s_addr;
			ret = nft_cmd_rule_check(h, chain, table, cs, verbose);
		}
	}

	return ret;
}

static int
nft_arp_replace_entry(struct nft_handle *h,
		      const char *chain, const char *table,
		      struct iptables_command_state *cs,
		      struct xtables_args *args, bool verbose,
		      int rulenum)
{
	cs->arp.arp.src.s_addr = args->s.addr.v4->s_addr;
	cs->arp.arp.tgt.s_addr = args->d.addr.v4->s_addr;
	cs->arp.arp.smsk.s_addr = args->s.mask.v4->s_addr;
	cs->arp.arp.tmsk.s_addr = args->d.mask.v4->s_addr;

	return nft_cmd_rule_replace(h, chain, table, cs, rulenum, verbose);
}

struct nft_family_ops nft_family_ops_arp = {
	.add			= nft_arp_add,
	.is_same		= nft_arp_is_same,
	.print_payload		= NULL,
	.parse_meta		= nft_arp_parse_meta,
	.parse_payload		= nft_arp_parse_payload,
	.print_header		= nft_arp_print_header,
	.print_rule		= nft_arp_print_rule,
	.save_rule		= nft_arp_save_rule,
	.save_chain		= nft_arp_save_chain,
	.cmd_parse		= {
		.post_parse	= nft_arp_post_parse,
	},
	.rule_to_cs		= nft_rule_to_iptables_command_state,
	.init_cs		= nft_arp_init_cs,
	.clear_cs		= nft_clear_iptables_command_state,
	.parse_target		= nft_ipv46_parse_target,
	.add_entry		= nft_arp_add_entry,
	.delete_entry		= nft_arp_delete_entry,
	.check_entry		= nft_arp_check_entry,
	.replace_entry		= nft_arp_replace_entry,
};
