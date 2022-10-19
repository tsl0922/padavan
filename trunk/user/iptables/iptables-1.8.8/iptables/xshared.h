#ifndef IPTABLES_XSHARED_H
#define IPTABLES_XSHARED_H 1

#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <netinet/in.h>
#include <net/if.h>
#include <linux/netfilter_arp/arp_tables.h>
#include <linux/netfilter_ipv4/ip_tables.h>
#include <linux/netfilter_ipv6/ip6_tables.h>

#ifdef DEBUG
#define DEBUGP(x, args...) fprintf(stderr, x, ## args)
#else
#define DEBUGP(x, args...)
#endif

enum {
	OPT_NONE        = 0,
	OPT_NUMERIC     = 1 << 0,
	OPT_SOURCE      = 1 << 1,
	OPT_DESTINATION = 1 << 2,
	OPT_PROTOCOL    = 1 << 3,
	OPT_JUMP        = 1 << 4,
	OPT_VERBOSE     = 1 << 5,
	OPT_EXPANDED    = 1 << 6,
	OPT_VIANAMEIN   = 1 << 7,
	OPT_VIANAMEOUT  = 1 << 8,
	OPT_LINENUMBERS = 1 << 9,
	OPT_COUNTERS    = 1 << 10,
	OPT_FRAGMENT	= 1 << 11,
	/* below are for arptables only */
	OPT_S_MAC	= 1 << 12,
	OPT_D_MAC	= 1 << 13,
	OPT_H_LENGTH	= 1 << 14,
	OPT_OPCODE	= 1 << 15,
	OPT_H_TYPE	= 1 << 16,
	OPT_P_TYPE	= 1 << 17,
};

#define NUMBER_OF_OPT	ARRAY_SIZE(optflags)
static const char optflags[]
= { 'n', 's', 'd', 'p', 'j', 'v', 'x', 'i', 'o', '0', 'c', 'f', 2, 3, 'l', 4, 5, 6 };

enum {
	CMD_NONE		= 0,
	CMD_INSERT		= 1 << 0,
	CMD_DELETE		= 1 << 1,
	CMD_DELETE_NUM		= 1 << 2,
	CMD_REPLACE		= 1 << 3,
	CMD_APPEND		= 1 << 4,
	CMD_LIST		= 1 << 5,
	CMD_FLUSH		= 1 << 6,
	CMD_ZERO		= 1 << 7,
	CMD_NEW_CHAIN		= 1 << 8,
	CMD_DELETE_CHAIN	= 1 << 9,
	CMD_SET_POLICY		= 1 << 10,
	CMD_RENAME_CHAIN	= 1 << 11,
	CMD_LIST_RULES		= 1 << 12,
	CMD_ZERO_NUM		= 1 << 13,
	CMD_CHECK		= 1 << 14,
};
#define NUMBER_OF_CMD		16

struct xtables_globals;
struct xtables_rule_match;
struct xtables_target;

#define OPTSTRING_COMMON "-:A:C:D:E:F::I:L::M:N:P:VX::Z::" "c:d:i:j:o:p:s:t:"
#define IPT_OPTSTRING	OPTSTRING_COMMON "R:S::W::" "46bfg:h::m:nvw::x"
#define ARPT_OPTSTRING	OPTSTRING_COMMON "R:S::" "h::l:nv" /* "m:" */
#define EBT_OPTSTRING	OPTSTRING_COMMON "hv"

/* define invflags which won't collide with IPT ones */
#define IPT_INV_SRCDEVADDR	0x0080
#define IPT_INV_TGTDEVADDR	0x0100
#define IPT_INV_ARPHLN		0x0200
#define IPT_INV_ARPOP		0x0400
#define IPT_INV_ARPHRD		0x0800

void
set_option(unsigned int *options, unsigned int option, u_int16_t *invflg,
	   bool invert);

/**
 * xtables_afinfo - protocol family dependent information
 * @kmod:		kernel module basename (e.g. "ip_tables")
 * @proc_exists:	file which exists in procfs when module already loaded
 * @libprefix:		prefix of .so library name (e.g. "libipt_")
 * @family:		nfproto family
 * @ipproto:		used by setsockopt (e.g. IPPROTO_IP)
 * @so_rev_match:	optname to check revision support of match
 * @so_rev_target:	optname to check revision support of target
 */
struct xtables_afinfo {
	const char *kmod;
	const char *proc_exists;
	const char *libprefix;
	uint8_t family;
	uint8_t ipproto;
	int so_rev_match;
	int so_rev_target;
};

/* trick for ebtables-compat, since watchers are targets */
struct ebt_match {
	struct ebt_match			*next;
	union {
		struct xtables_match		*match;
		struct xtables_target		*watcher;
	} u;
	bool					ismatch;
};

/* Fake ebt_entry */
struct ebt_entry {
	/* this needs to be the first field */
	unsigned int bitmask;
	unsigned int invflags;
	uint16_t ethproto;
	/* the physical in-dev */
	char in[IFNAMSIZ];
	/* the logical in-dev */
	char logical_in[IFNAMSIZ];
	/* the physical out-dev */
	char out[IFNAMSIZ];
	/* the logical out-dev */
	char logical_out[IFNAMSIZ];
	unsigned char sourcemac[6];
	unsigned char sourcemsk[6];
	unsigned char destmac[6];
	unsigned char destmsk[6];
};

struct iptables_command_state {
	union {
		struct ebt_entry eb;
		struct ipt_entry fw;
		struct ip6t_entry fw6;
		struct arpt_entry arp;
	};
	int c;
	unsigned int options;
	struct xtables_rule_match *matches;
	struct ebt_match *match_list;
	struct xtables_target *target;
	struct xt_counters counters;
	char *protocol;
	int proto_used;
	const char *jumpto;
	char **argv;
	bool restore;
};

typedef int (*mainfunc_t)(int, char **);

struct subcommand {
	const char *name;
	mainfunc_t main;
};

enum {
	XT_OPTION_OFFSET_SCALE = 256,
};

extern void print_extension_helps(const struct xtables_target *,
	const struct xtables_rule_match *);
extern int command_default(struct iptables_command_state *,
	struct xtables_globals *, bool invert);
extern int subcmd_main(int, char **, const struct subcommand *);
extern void xs_init_target(struct xtables_target *);
extern void xs_init_match(struct xtables_match *);

/**
 * Values for the iptables lock.
 *
 * A value >= 0 indicates the lock filedescriptor. Other values are:
 *
 * XT_LOCK_FAILED : The lock could not be acquired.
 *
 * XT_LOCK_BUSY : The lock was held by another process. xtables_lock only
 * returns this value when |wait| == false. If |wait| == true, xtables_lock
 * will not return unless the lock has been acquired.
 *
 * XT_LOCK_NOT_ACQUIRED : We have not yet attempted to acquire the lock.
 */
enum {
	XT_LOCK_BUSY = -1,
	XT_LOCK_FAILED = -2,
	XT_LOCK_NOT_ACQUIRED  = -3,
};
extern void xtables_unlock(int lock);
extern int xtables_lock_or_exit(int wait);

int parse_wait_time(int argc, char *argv[]);
void parse_wait_interval(int argc, char *argv[]);
int parse_counters(const char *string, struct xt_counters *ctr);
bool tokenize_rule_counters(char **bufferp, char **pcnt, char **bcnt, int line);
bool xs_has_arg(int argc, char *argv[]);

extern const struct xtables_afinfo *afinfo;

#define MAX_ARGC	255
struct argv_store {
	int argc;
	char *argv[MAX_ARGC];
	int argvattr[MAX_ARGC];
};

void add_argv(struct argv_store *store, const char *what, int quoted);
void free_argv(struct argv_store *store);
void save_argv(struct argv_store *dst, struct argv_store *src);
void add_param_to_argv(struct argv_store *store, char *parsestart, int line);
#ifdef DEBUG
void debug_print_argv(struct argv_store *store);
#else
#  define debug_print_argv(...) /* nothing */
#endif

const char *ipv4_addr_to_string(const struct in_addr *addr,
				const struct in_addr *mask,
				unsigned int format);
void print_header(unsigned int format, const char *chain, const char *pol,
		  const struct xt_counters *counters,
		  int refs, uint32_t entries);
void print_ipv4_addresses(const struct ipt_entry *fw, unsigned int format);
void save_ipv4_addr(char letter, const struct in_addr *addr,
		    const struct in_addr *mask, int invert);
void print_ipv6_addresses(const struct ip6t_entry *fw6, unsigned int format);
void save_ipv6_addr(char letter, const struct in6_addr *addr,
		    const struct in6_addr *mask, int invert);

void print_ifaces(const char *iniface, const char *outiface, uint8_t invflags,
		  unsigned int format);
void save_iface(char letter, const char *iface,
		const unsigned char *mask, int invert);

void print_fragment(unsigned int flags, unsigned int invflags,
		    unsigned int format, bool fake);

void command_match(struct iptables_command_state *cs, bool invert);
const char *xt_parse_target(const char *targetname);
void command_jump(struct iptables_command_state *cs, const char *jumpto);

char cmd2char(int option);
void add_command(unsigned int *cmd, const int newcmd,
		 const int othercmds, int invert);
int parse_rulenumber(const char *rule);
void assert_valid_chain_name(const char *chainname);

void generic_opt_check(int command, int options);
char opt2char(int option);

void print_rule_details(unsigned int linenum, const struct xt_counters *ctrs,
			const char *targname, uint8_t proto, uint8_t flags,
			uint8_t invflags, unsigned int format);
void save_rule_details(const char *iniface, unsigned const char *iniface_mask,
		       const char *outiface, unsigned const char *outiface_mask,
		       uint16_t proto, int frag, uint8_t invflags);

int print_match_save(const struct xt_entry_match *e, const void *ip);

void exit_tryhelp(int status, int line) __attribute__((noreturn));

struct addr_mask {
	union {
		struct in_addr	*v4;
		struct in6_addr *v6;
		void *ptr;
	} addr;

	unsigned int naddrs;

	union {
		struct in_addr	*v4;
		struct in6_addr *v6;
		void *ptr;
	} mask;
};

struct xtables_args {
	int		family;
	uint16_t	proto;
	uint8_t		flags;
	uint16_t	invflags;
	char		iniface[IFNAMSIZ], outiface[IFNAMSIZ];
	unsigned char	iniface_mask[IFNAMSIZ], outiface_mask[IFNAMSIZ];
	bool		goto_set;
	const char	*shostnetworkmask, *dhostnetworkmask;
	const char	*pcnt, *bcnt;
	struct addr_mask s, d;
	const char	*src_mac, *dst_mac;
	const char	*arp_hlen, *arp_opcode;
	const char	*arp_htype, *arp_ptype;
	unsigned long long pcnt_cnt, bcnt_cnt;
	int		wait;
};

struct xt_cmd_parse_ops {
	void	(*proto_parse)(struct iptables_command_state *cs,
			       struct xtables_args *args);
	void	(*post_parse)(int command,
			      struct iptables_command_state *cs,
			      struct xtables_args *args);
};

struct xt_cmd_parse {
	unsigned int			command;
	unsigned int			rulenum;
	char				*table;
	const char			*chain;
	const char			*newname;
	const char			*policy;
	bool				restore;
	int				line;
	int				verbose;
	bool				xlate;
	struct xt_cmd_parse_ops		*ops;
};

void do_parse(int argc, char *argv[],
	      struct xt_cmd_parse *p, struct iptables_command_state *cs,
	      struct xtables_args *args);

void ipv4_proto_parse(struct iptables_command_state *cs,
		      struct xtables_args *args);
void ipv6_proto_parse(struct iptables_command_state *cs,
		      struct xtables_args *args);
void ipv4_post_parse(int command, struct iptables_command_state *cs,
		     struct xtables_args *args);
void ipv6_post_parse(int command, struct iptables_command_state *cs,
		     struct xtables_args *args);

extern char *arp_opcodes[];
#define ARP_NUMOPCODES 9

#endif /* IPTABLES_XSHARED_H */
