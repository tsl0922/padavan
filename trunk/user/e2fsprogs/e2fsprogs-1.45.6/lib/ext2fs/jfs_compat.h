
#ifndef _JFS_COMPAT_H
#define _JFS_COMPAT_H

#include "kernel-list.h"
#include <errno.h>
#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef HAVE_WINSOCK_H
#include <winsock.h>
#else
#include <arpa/inet.h>
#endif

#define printk printf
#define KERN_ERR ""
#define KERN_DEBUG ""

#define READ 0
#define WRITE 1

#define cpu_to_be32(n) htonl(n)
#define be32_to_cpu(n) ntohl(n)
#define cpu_to_be16(n) htons(n)
#define be16_to_cpu(n) ntohs(n)

typedef unsigned int tid_t;
typedef struct journal_s journal_t;
typedef struct kdev_s *kdev_t;

struct buffer_head;
struct inode;

#define GFP_KERNEL	0
#define JFS_TAG_SIZE32	JBD_TAG_SIZE32
#define JFS_BARRIER	0
typedef __u64 u64;
#define JFS_CRC32_CHKSUM	JBD2_CRC32_CHKSUM
#define JFS_CRC32_CHKSUM_SIZE	JBD2_CRC32_CHKSUM_SIZE
#define put_bh(x)	brelse(x)
#define be64_to_cpu(x)	ext2fs_be64_to_cpu(x)

static inline __u32 jbd2_chksum(journal_t *j EXT2FS_ATTR((unused)),
				__u32 crc, const void *address,
				unsigned int length)
{
	return ext2fs_crc32c_le(crc, address, length);
}
#define crc32_be(x, y, z)	ext2fs_crc32_be((x), (y), (z))
#define spin_lock_init(x)
#define spin_lock(x)
#define spin_unlock(x)
#define yield()
#define SLAB_HWCACHE_ALIGN	0
#define SLAB_TEMPORARY		0
#define KMEM_CACHE(__struct, __flags) kmem_cache_create(#__struct,\
                sizeof(struct __struct), __alignof__(struct __struct),\
                (__flags), NULL)

#define blkdev_issue_flush(kdev, a, b)	sync_blockdev(kdev)
#define is_power_of_2(x)	((x) != 0 && (((x) & ((x) - 1)) == 0))

struct journal_s
{
	unsigned long		j_flags;
	int			j_errno;
	struct buffer_head *	j_sb_buffer;
	struct journal_superblock_s *j_superblock;
	int			j_format_version;
	unsigned long		j_head;
	unsigned long		j_tail;
	unsigned long		j_free;
	unsigned long		j_first, j_last;
	kdev_t			j_dev;
	kdev_t			j_fs_dev;
	int			j_blocksize;
	unsigned int		j_blk_offset;
	unsigned int		j_maxlen;
	struct inode *		j_inode;
	tid_t			j_tail_sequence;
	tid_t			j_transaction_sequence;
	__u8			j_uuid[16];
	struct jbd2_revoke_table_s *j_revoke;
	struct jbd2_revoke_table_s *j_revoke_table[2];
	tid_t			j_failed_commit;
	__u32			j_csum_seed;
};

#define is_journal_abort(x) 0

#define BUFFER_TRACE(bh, info)	do {} while (0)

/* Need this so we can compile with configure --enable-gcc-wall */
#ifdef NO_INLINE_FUNCS
#define inline
#endif

#endif /* _JFS_COMPAT_H */
