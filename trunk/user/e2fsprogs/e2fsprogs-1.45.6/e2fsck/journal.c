/*
 * journal.c --- code for handling the "ext3" journal
 *
 * Copyright (C) 2000 Andreas Dilger
 * Copyright (C) 2000 Theodore Ts'o
 *
 * Parts of the code are based on fs/jfs/journal.c by Stephen C. Tweedie
 * Copyright (C) 1999 Red Hat Software
 *
 * This file may be redistributed under the terms of the
 * GNU General Public License version 2 or at your discretion
 * any later version.
 */

#include "config.h"
#ifdef HAVE_SYS_MOUNT_H
#include <sys/param.h>
#include <sys/mount.h>
#define MNT_FL (MS_MGC_VAL | MS_RDONLY)
#endif
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif

#define E2FSCK_INCLUDE_INLINE_FUNCS
#include "jfs_user.h"
#include "problem.h"
#include "uuid/uuid.h"

#ifdef CONFIG_JBD_DEBUG		/* Enabled by configure --enable-jfs-debug */
static int bh_count = 0;
#endif

/*
 * Define USE_INODE_IO to use the inode_io.c / fileio.c codepaths.
 * This creates a larger static binary, and a smaller binary using
 * shared libraries.  It's also probably slightly less CPU-efficient,
 * which is why it's not on by default.  But, it's a good way of
 * testing the functions in inode_io.c and fileio.c.
 */
#undef USE_INODE_IO

/* Checksumming functions */
static int e2fsck_journal_verify_csum_type(journal_t *j,
					   journal_superblock_t *jsb)
{
	if (!journal_has_csum_v2or3(j))
		return 1;

	return jsb->s_checksum_type == JBD2_CRC32C_CHKSUM;
}

static __u32 e2fsck_journal_sb_csum(journal_superblock_t *jsb)
{
	__u32 crc, old_crc;

	old_crc = jsb->s_checksum;
	jsb->s_checksum = 0;
	crc = ext2fs_crc32c_le(~0, (unsigned char *)jsb,
			       sizeof(journal_superblock_t));
	jsb->s_checksum = old_crc;

	return crc;
}

static int e2fsck_journal_sb_csum_verify(journal_t *j,
					 journal_superblock_t *jsb)
{
	__u32 provided, calculated;

	if (!journal_has_csum_v2or3(j))
		return 1;

	provided = ext2fs_be32_to_cpu(jsb->s_checksum);
	calculated = e2fsck_journal_sb_csum(jsb);

	return provided == calculated;
}

static errcode_t e2fsck_journal_sb_csum_set(journal_t *j,
					    journal_superblock_t *jsb)
{
	__u32 crc;

	if (!journal_has_csum_v2or3(j))
		return 0;

	crc = e2fsck_journal_sb_csum(jsb);
	jsb->s_checksum = ext2fs_cpu_to_be32(crc);
	return 0;
}

/* Kernel compatibility functions for handling the journal.  These allow us
 * to use the recovery.c file virtually unchanged from the kernel, so we
 * don't have to do much to keep kernel and user recovery in sync.
 */
int journal_bmap(journal_t *journal, blk64_t block, unsigned long long *phys)
{
#ifdef USE_INODE_IO
	*phys = block;
	return 0;
#else
	struct inode 	*inode = journal->j_inode;
	errcode_t	retval;
	blk64_t		pblk;

	if (!inode) {
		*phys = block;
		return 0;
	}

	retval= ext2fs_bmap2(inode->i_ctx->fs, inode->i_ino,
			     &inode->i_ext2, NULL, 0, block, 0, &pblk);
	*phys = pblk;
	return -1 * ((int) retval);
#endif
}

struct buffer_head *getblk(kdev_t kdev, blk64_t blocknr, int blocksize)
{
	struct buffer_head *bh;
	int bufsize = sizeof(*bh) + kdev->k_ctx->fs->blocksize -
		sizeof(bh->b_data);

	bh = e2fsck_allocate_memory(kdev->k_ctx, bufsize, "block buffer");
	if (!bh)
		return NULL;

#ifdef CONFIG_JBD_DEBUG
	if (journal_enable_debug >= 3)
		bh_count++;
#endif
	jfs_debug(4, "getblk for block %llu (%d bytes)(total %d)\n",
		  (unsigned long long) blocknr, blocksize, bh_count);

	bh->b_ctx = kdev->k_ctx;
	if (kdev->k_dev == K_DEV_FS)
		bh->b_io = kdev->k_ctx->fs->io;
	else
		bh->b_io = kdev->k_ctx->journal_io;
	bh->b_size = blocksize;
	bh->b_blocknr = blocknr;

	return bh;
}

int sync_blockdev(kdev_t kdev)
{
	io_channel	io;

	if (kdev->k_dev == K_DEV_FS)
		io = kdev->k_ctx->fs->io;
	else
		io = kdev->k_ctx->journal_io;

	return io_channel_flush(io) ? -EIO : 0;
}

void ll_rw_block(int rw, int nr, struct buffer_head *bhp[])
{
	errcode_t retval;
	struct buffer_head *bh;

	for (; nr > 0; --nr) {
		bh = *bhp++;
		if (rw == READ && !bh->b_uptodate) {
			jfs_debug(3, "reading block %llu/%p\n",
				  bh->b_blocknr, (void *) bh);
			retval = io_channel_read_blk64(bh->b_io,
						     bh->b_blocknr,
						     1, bh->b_data);
			if (retval) {
				com_err(bh->b_ctx->device_name, retval,
					"while reading block %llu\n",
					bh->b_blocknr);
				bh->b_err = (int) retval;
				continue;
			}
			bh->b_uptodate = 1;
		} else if (rw == WRITE && bh->b_dirty) {
			jfs_debug(3, "writing block %llu/%p\n",
				  bh->b_blocknr,
				  (void *) bh);
			retval = io_channel_write_blk64(bh->b_io,
						      bh->b_blocknr,
						      1, bh->b_data);
			if (retval) {
				com_err(bh->b_ctx->device_name, retval,
					"while writing block %llu\n",
					bh->b_blocknr);
				bh->b_err = (int) retval;
				continue;
			}
			bh->b_dirty = 0;
			bh->b_uptodate = 1;
		} else {
			jfs_debug(3, "no-op %s for block %llu\n",
				  rw == READ ? "read" : "write",
				  bh->b_blocknr);
		}
	}
}

void mark_buffer_dirty(struct buffer_head *bh)
{
	bh->b_dirty = 1;
}

static void mark_buffer_clean(struct buffer_head * bh)
{
	bh->b_dirty = 0;
}

void brelse(struct buffer_head *bh)
{
	if (bh->b_dirty)
		ll_rw_block(WRITE, 1, &bh);
	jfs_debug(3, "freeing block %llu/%p (total %d)\n",
		  bh->b_blocknr, (void *) bh, --bh_count);
	ext2fs_free_mem(&bh);
}

int buffer_uptodate(struct buffer_head *bh)
{
	return bh->b_uptodate;
}

void mark_buffer_uptodate(struct buffer_head *bh, int val)
{
	bh->b_uptodate = val;
}

void wait_on_buffer(struct buffer_head *bh)
{
	if (!bh->b_uptodate)
		ll_rw_block(READ, 1, &bh);
}


static void e2fsck_clear_recover(e2fsck_t ctx, int error)
{
	ext2fs_clear_feature_journal_needs_recovery(ctx->fs->super);

	/* if we had an error doing journal recovery, we need a full fsck */
	if (error)
		ctx->fs->super->s_state &= ~EXT2_VALID_FS;
	ext2fs_mark_super_dirty(ctx->fs);
}

/*
 * This is a helper function to check the validity of the journal.
 */
struct process_block_struct {
	e2_blkcnt_t	last_block;
};

static int process_journal_block(ext2_filsys fs,
				 blk64_t	*block_nr,
				 e2_blkcnt_t blockcnt,
				 blk64_t ref_block EXT2FS_ATTR((unused)),
				 int ref_offset EXT2FS_ATTR((unused)),
				 void *priv_data)
{
	struct process_block_struct *p;
	blk64_t	blk = *block_nr;

	p = (struct process_block_struct *) priv_data;

	if (!blk || blk < fs->super->s_first_data_block ||
	    blk >= ext2fs_blocks_count(fs->super))
		return BLOCK_ABORT;

	if (blockcnt >= 0)
		p->last_block = blockcnt;
	return 0;
}

static errcode_t e2fsck_get_journal(e2fsck_t ctx, journal_t **ret_journal)
{
	struct process_block_struct pb;
	struct ext2_super_block *sb = ctx->fs->super;
	struct ext2_super_block jsuper;
	struct problem_context	pctx;
	struct buffer_head 	*bh;
	struct inode		*j_inode = NULL;
	struct kdev_s		*dev_fs = NULL, *dev_journal;
	const char		*journal_name = 0;
	journal_t		*journal = NULL;
	errcode_t		retval = 0;
	io_manager		io_ptr = 0;
	unsigned long long	start = 0;
	int			ret;
	int			ext_journal = 0;
	int			tried_backup_jnl = 0;

	clear_problem_context(&pctx);

	journal = e2fsck_allocate_memory(ctx, sizeof(journal_t), "journal");
	if (!journal) {
		return EXT2_ET_NO_MEMORY;
	}

	dev_fs = e2fsck_allocate_memory(ctx, 2*sizeof(struct kdev_s), "kdev");
	if (!dev_fs) {
		retval = EXT2_ET_NO_MEMORY;
		goto errout;
	}
	dev_journal = dev_fs+1;

	dev_fs->k_ctx = dev_journal->k_ctx = ctx;
	dev_fs->k_dev = K_DEV_FS;
	dev_journal->k_dev = K_DEV_JOURNAL;

	journal->j_dev = dev_journal;
	journal->j_fs_dev = dev_fs;
	journal->j_inode = NULL;
	journal->j_blocksize = ctx->fs->blocksize;

	if (uuid_is_null(sb->s_journal_uuid)) {
		if (!sb->s_journal_inum) {
			retval = EXT2_ET_BAD_INODE_NUM;
			goto errout;
		}
		j_inode = e2fsck_allocate_memory(ctx, sizeof(*j_inode),
						 "journal inode");
		if (!j_inode) {
			retval = EXT2_ET_NO_MEMORY;
			goto errout;
		}

		j_inode->i_ctx = ctx;
		j_inode->i_ino = sb->s_journal_inum;

		if ((retval = ext2fs_read_inode(ctx->fs,
						sb->s_journal_inum,
						&j_inode->i_ext2))) {
		try_backup_journal:
			if (sb->s_jnl_backup_type != EXT3_JNL_BACKUP_BLOCKS ||
			    tried_backup_jnl)
				goto errout;
			memset(&j_inode->i_ext2, 0, sizeof(struct ext2_inode));
			memcpy(&j_inode->i_ext2.i_block[0], sb->s_jnl_blocks,
			       EXT2_N_BLOCKS*4);
			j_inode->i_ext2.i_size_high = sb->s_jnl_blocks[15];
			j_inode->i_ext2.i_size = sb->s_jnl_blocks[16];
			j_inode->i_ext2.i_links_count = 1;
			j_inode->i_ext2.i_mode = LINUX_S_IFREG | 0600;
			e2fsck_use_inode_shortcuts(ctx, 1);
			ctx->stashed_ino = j_inode->i_ino;
			ctx->stashed_inode = &j_inode->i_ext2;
			tried_backup_jnl++;
		}
		if (!j_inode->i_ext2.i_links_count ||
		    !LINUX_S_ISREG(j_inode->i_ext2.i_mode)) {
			retval = EXT2_ET_NO_JOURNAL;
			goto try_backup_journal;
		}
		if (EXT2_I_SIZE(&j_inode->i_ext2) / journal->j_blocksize <
		    JFS_MIN_JOURNAL_BLOCKS) {
			retval = EXT2_ET_JOURNAL_TOO_SMALL;
			goto try_backup_journal;
		}
		pb.last_block = -1;
		retval = ext2fs_block_iterate3(ctx->fs, j_inode->i_ino,
					       BLOCK_FLAG_HOLE, 0,
					       process_journal_block, &pb);
		if ((pb.last_block + 1) * ctx->fs->blocksize <
		    (int) EXT2_I_SIZE(&j_inode->i_ext2)) {
			retval = EXT2_ET_JOURNAL_TOO_SMALL;
			goto try_backup_journal;
		}
		if (tried_backup_jnl && !(ctx->options & E2F_OPT_READONLY)) {
			retval = ext2fs_write_inode(ctx->fs, sb->s_journal_inum,
						    &j_inode->i_ext2);
			if (retval)
				goto errout;
		}

		journal->j_maxlen = EXT2_I_SIZE(&j_inode->i_ext2) /
			journal->j_blocksize;

#ifdef USE_INODE_IO
		retval = ext2fs_inode_io_intern2(ctx->fs, sb->s_journal_inum,
						 &j_inode->i_ext2,
						 &journal_name);
		if (retval)
			goto errout;

		io_ptr = inode_io_manager;
#else
		journal->j_inode = j_inode;
		ctx->journal_io = ctx->fs->io;
		if ((ret = journal_bmap(journal, 0, &start)) != 0) {
			retval = (errcode_t) (-1 * ret);
			goto errout;
		}
#endif
	} else {
		ext_journal = 1;
		if (!ctx->journal_name) {
			char uuid[37];

			uuid_unparse(sb->s_journal_uuid, uuid);
			ctx->journal_name = blkid_get_devname(ctx->blkid,
							      "UUID", uuid);
			if (!ctx->journal_name)
				ctx->journal_name = blkid_devno_to_devname(sb->s_journal_dev);
		}
		journal_name = ctx->journal_name;

		if (!journal_name) {
			fix_problem(ctx, PR_0_CANT_FIND_JOURNAL, &pctx);
			retval = EXT2_ET_LOAD_EXT_JOURNAL;
			goto errout;
		}

		jfs_debug(1, "Using journal file %s\n", journal_name);
		io_ptr = unix_io_manager;
	}

#if 0
	test_io_backing_manager = io_ptr;
	io_ptr = test_io_manager;
#endif
#ifndef USE_INODE_IO
	if (ext_journal)
#endif
	{
		int flags = IO_FLAG_RW;
		if (!(ctx->mount_flags & EXT2_MF_ISROOT &&
		      ctx->mount_flags & EXT2_MF_READONLY))
			flags |= IO_FLAG_EXCLUSIVE;
		if ((ctx->mount_flags & EXT2_MF_READONLY) &&
		    (ctx->options & E2F_OPT_FORCE))
			flags &= ~IO_FLAG_EXCLUSIVE;


		retval = io_ptr->open(journal_name, flags,
				      &ctx->journal_io);
	}
	if (retval)
		goto errout;

	io_channel_set_blksize(ctx->journal_io, ctx->fs->blocksize);

	if (ext_journal) {
		blk64_t maxlen;

		start = ext2fs_journal_sb_start(ctx->fs->blocksize) - 1;
		bh = getblk(dev_journal, start, ctx->fs->blocksize);
		if (!bh) {
			retval = EXT2_ET_NO_MEMORY;
			goto errout;
		}
		ll_rw_block(READ, 1, &bh);
		if ((retval = bh->b_err) != 0) {
			brelse(bh);
			goto errout;
		}
		memcpy(&jsuper, start ? bh->b_data :  bh->b_data + SUPERBLOCK_OFFSET,
		       sizeof(jsuper));
#ifdef WORDS_BIGENDIAN
		if (jsuper.s_magic == ext2fs_swab16(EXT2_SUPER_MAGIC))
			ext2fs_swap_super(&jsuper);
#endif
		if (jsuper.s_magic != EXT2_SUPER_MAGIC ||
		    !ext2fs_has_feature_journal_dev(&jsuper)) {
			fix_problem(ctx, PR_0_EXT_JOURNAL_BAD_SUPER, &pctx);
			retval = EXT2_ET_LOAD_EXT_JOURNAL;
			brelse(bh);
			goto errout;
		}
		/* Make sure the journal UUID is correct */
		if (memcmp(jsuper.s_uuid, ctx->fs->super->s_journal_uuid,
			   sizeof(jsuper.s_uuid))) {
			fix_problem(ctx, PR_0_JOURNAL_BAD_UUID, &pctx);
			retval = EXT2_ET_LOAD_EXT_JOURNAL;
			brelse(bh);
			goto errout;
		}

		/* Check the superblock checksum */
		if (ext2fs_has_feature_metadata_csum(&jsuper)) {
			struct struct_ext2_filsys fsx;
			struct ext2_super_block	superx;
			void *p;

			p = start ? bh->b_data : bh->b_data + SUPERBLOCK_OFFSET;
			memcpy(&fsx, ctx->fs, sizeof(fsx));
			memcpy(&superx, ctx->fs->super, sizeof(superx));
			fsx.super = &superx;
			ext2fs_set_feature_metadata_csum(fsx.super);
			if (!ext2fs_superblock_csum_verify(&fsx, p) &&
			    fix_problem(ctx, PR_0_EXT_JOURNAL_SUPER_CSUM_INVALID,
					&pctx)) {
				ext2fs_superblock_csum_set(&fsx, p);
				mark_buffer_dirty(bh);
			}
		}
		brelse(bh);

		maxlen = ext2fs_blocks_count(&jsuper);
		journal->j_maxlen = (maxlen < 1ULL << 32) ? maxlen : (1ULL << 32) - 1;
		start++;
	}

	if (!(bh = getblk(dev_journal, start, journal->j_blocksize))) {
		retval = EXT2_ET_NO_MEMORY;
		goto errout;
	}

	journal->j_sb_buffer = bh;
	journal->j_superblock = (journal_superblock_t *)bh->b_data;

#ifdef USE_INODE_IO
	if (j_inode)
		ext2fs_free_mem(&j_inode);
#endif

	*ret_journal = journal;
	e2fsck_use_inode_shortcuts(ctx, 0);
	return 0;

errout:
	e2fsck_use_inode_shortcuts(ctx, 0);
	if (dev_fs)
		ext2fs_free_mem(&dev_fs);
	if (j_inode)
		ext2fs_free_mem(&j_inode);
	if (journal)
		ext2fs_free_mem(&journal);
	return retval;
}

static errcode_t e2fsck_journal_fix_bad_inode(e2fsck_t ctx,
					      struct problem_context *pctx)
{
	struct ext2_super_block *sb = ctx->fs->super;
	int recover = ext2fs_has_feature_journal_needs_recovery(ctx->fs->super);
	int has_journal = ext2fs_has_feature_journal(ctx->fs->super);

	if (has_journal || sb->s_journal_inum) {
		/* The journal inode is bogus, remove and force full fsck */
		pctx->ino = sb->s_journal_inum;
		if (fix_problem(ctx, PR_0_JOURNAL_BAD_INODE, pctx)) {
			if (has_journal && sb->s_journal_inum)
				printf("*** journal has been deleted ***\n\n");
			ext2fs_clear_feature_journal(sb);
			sb->s_journal_inum = 0;
			memset(sb->s_jnl_blocks, 0, sizeof(sb->s_jnl_blocks));
			ctx->flags |= E2F_FLAG_JOURNAL_INODE;
			ctx->fs->flags &= ~EXT2_FLAG_MASTER_SB_ONLY;
			e2fsck_clear_recover(ctx, 1);
			return 0;
		}
		return EXT2_ET_CORRUPT_JOURNAL_SB;
	} else if (recover) {
		if (fix_problem(ctx, PR_0_JOURNAL_RECOVER_SET, pctx)) {
			e2fsck_clear_recover(ctx, 1);
			return 0;
		}
		return EXT2_ET_UNSUPP_FEATURE;
	}
	return 0;
}

#define V1_SB_SIZE	0x0024
static void clear_v2_journal_fields(journal_t *journal)
{
	e2fsck_t ctx = journal->j_dev->k_ctx;
	struct problem_context pctx;

	clear_problem_context(&pctx);

	if (!fix_problem(ctx, PR_0_CLEAR_V2_JOURNAL, &pctx))
		return;

	ctx->flags |= E2F_FLAG_PROBLEMS_FIXED;
	memset(((char *) journal->j_superblock) + V1_SB_SIZE, 0,
	       ctx->fs->blocksize-V1_SB_SIZE);
	mark_buffer_dirty(journal->j_sb_buffer);
}


static errcode_t e2fsck_journal_load(journal_t *journal)
{
	e2fsck_t ctx = journal->j_dev->k_ctx;
	journal_superblock_t *jsb;
	struct buffer_head *jbh = journal->j_sb_buffer;
	struct problem_context pctx;

	clear_problem_context(&pctx);

	ll_rw_block(READ, 1, &jbh);
	if (jbh->b_err) {
		com_err(ctx->device_name, jbh->b_err, "%s",
			_("reading journal superblock\n"));
		return jbh->b_err;
	}

	jsb = journal->j_superblock;
	/* If we don't even have JFS_MAGIC, we probably have a wrong inode */
	if (jsb->s_header.h_magic != htonl(JFS_MAGIC_NUMBER))
		return e2fsck_journal_fix_bad_inode(ctx, &pctx);

	switch (ntohl(jsb->s_header.h_blocktype)) {
	case JFS_SUPERBLOCK_V1:
		journal->j_format_version = 1;
		if (jsb->s_feature_compat ||
		    jsb->s_feature_incompat ||
		    jsb->s_feature_ro_compat ||
		    jsb->s_nr_users)
			clear_v2_journal_fields(journal);
		break;

	case JFS_SUPERBLOCK_V2:
		journal->j_format_version = 2;
		if (ntohl(jsb->s_nr_users) > 1 &&
		    uuid_is_null(ctx->fs->super->s_journal_uuid))
			clear_v2_journal_fields(journal);
		if (ntohl(jsb->s_nr_users) > 1) {
			fix_problem(ctx, PR_0_JOURNAL_UNSUPP_MULTIFS, &pctx);
			return EXT2_ET_JOURNAL_UNSUPP_VERSION;
		}
		break;

	/*
	 * These should never appear in a journal super block, so if
	 * they do, the journal is badly corrupted.
	 */
	case JFS_DESCRIPTOR_BLOCK:
	case JFS_COMMIT_BLOCK:
	case JFS_REVOKE_BLOCK:
		return EXT2_ET_CORRUPT_JOURNAL_SB;

	/* If we don't understand the superblock major type, but there
	 * is a magic number, then it is likely to be a new format we
	 * just don't understand, so leave it alone. */
	default:
		return EXT2_ET_JOURNAL_UNSUPP_VERSION;
	}

	if (JFS_HAS_INCOMPAT_FEATURE(journal, ~JFS_KNOWN_INCOMPAT_FEATURES))
		return EXT2_ET_UNSUPP_FEATURE;

	if (JFS_HAS_RO_COMPAT_FEATURE(journal, ~JFS_KNOWN_ROCOMPAT_FEATURES))
		return EXT2_ET_RO_UNSUPP_FEATURE;

	/* Checksum v1-3 are mutually exclusive features. */
	if (jfs_has_feature_csum2(journal) && jfs_has_feature_csum3(journal))
		return EXT2_ET_CORRUPT_JOURNAL_SB;

	if (journal_has_csum_v2or3(journal) &&
	    jfs_has_feature_checksum(journal))
		return EXT2_ET_CORRUPT_JOURNAL_SB;

	if (!e2fsck_journal_verify_csum_type(journal, jsb) ||
	    !e2fsck_journal_sb_csum_verify(journal, jsb))
		return EXT2_ET_CORRUPT_JOURNAL_SB;

	if (journal_has_csum_v2or3(journal))
		journal->j_csum_seed = jbd2_chksum(journal, ~0, jsb->s_uuid,
						   sizeof(jsb->s_uuid));

	/* We have now checked whether we know enough about the journal
	 * format to be able to proceed safely, so any other checks that
	 * fail we should attempt to recover from. */
	if (jsb->s_blocksize != htonl(journal->j_blocksize)) {
		com_err(ctx->program_name, EXT2_ET_CORRUPT_JOURNAL_SB,
			_("%s: no valid journal superblock found\n"),
			ctx->device_name);
		return EXT2_ET_CORRUPT_JOURNAL_SB;
	}

	if (ntohl(jsb->s_maxlen) < journal->j_maxlen)
		journal->j_maxlen = ntohl(jsb->s_maxlen);
	else if (ntohl(jsb->s_maxlen) > journal->j_maxlen) {
		com_err(ctx->program_name, EXT2_ET_CORRUPT_JOURNAL_SB,
			_("%s: journal too short\n"),
			ctx->device_name);
		return EXT2_ET_CORRUPT_JOURNAL_SB;
	}

	journal->j_tail_sequence = ntohl(jsb->s_sequence);
	journal->j_transaction_sequence = journal->j_tail_sequence;
	journal->j_tail = ntohl(jsb->s_start);
	journal->j_first = ntohl(jsb->s_first);
	journal->j_last = ntohl(jsb->s_maxlen);

	return 0;
}

static void e2fsck_journal_reset_super(e2fsck_t ctx, journal_superblock_t *jsb,
				       journal_t *journal)
{
	char *p;
	union {
		uuid_t uuid;
		__u32 val[4];
	} u;
	__u32 new_seq = 0;
	int i;

	/* Leave a valid existing V1 superblock signature alone.
	 * Anything unrecognisable we overwrite with a new V2
	 * signature. */

	if (jsb->s_header.h_magic != htonl(JFS_MAGIC_NUMBER) ||
	    jsb->s_header.h_blocktype != htonl(JFS_SUPERBLOCK_V1)) {
		jsb->s_header.h_magic = htonl(JFS_MAGIC_NUMBER);
		jsb->s_header.h_blocktype = htonl(JFS_SUPERBLOCK_V2);
	}

	/* Zero out everything else beyond the superblock header */

	p = ((char *) jsb) + sizeof(journal_header_t);
	memset (p, 0, ctx->fs->blocksize-sizeof(journal_header_t));

	jsb->s_blocksize = htonl(ctx->fs->blocksize);
	jsb->s_maxlen = htonl(journal->j_maxlen);
	jsb->s_first = htonl(1);

	/* Initialize the journal sequence number so that there is "no"
	 * chance we will find old "valid" transactions in the journal.
	 * This avoids the need to zero the whole journal (slow to do,
	 * and risky when we are just recovering the filesystem).
	 */
	uuid_generate(u.uuid);
	for (i = 0; i < 4; i ++)
		new_seq ^= u.val[i];
	jsb->s_sequence = htonl(new_seq);
	e2fsck_journal_sb_csum_set(journal, jsb);

	mark_buffer_dirty(journal->j_sb_buffer);
	ll_rw_block(WRITE, 1, &journal->j_sb_buffer);
}

static errcode_t e2fsck_journal_fix_corrupt_super(e2fsck_t ctx,
						  journal_t *journal,
						  struct problem_context *pctx)
{
	struct ext2_super_block *sb = ctx->fs->super;
	int recover = ext2fs_has_feature_journal_needs_recovery(ctx->fs->super);

	if (ext2fs_has_feature_journal(sb)) {
		if (fix_problem(ctx, PR_0_JOURNAL_BAD_SUPER, pctx)) {
			e2fsck_journal_reset_super(ctx, journal->j_superblock,
						   journal);
			journal->j_transaction_sequence = 1;
			e2fsck_clear_recover(ctx, recover);
			return 0;
		}
		return EXT2_ET_CORRUPT_JOURNAL_SB;
	} else if (e2fsck_journal_fix_bad_inode(ctx, pctx))
		return EXT2_ET_CORRUPT_JOURNAL_SB;

	return 0;
}

static void e2fsck_journal_release(e2fsck_t ctx, journal_t *journal,
				   int reset, int drop)
{
	journal_superblock_t *jsb;

	if (drop)
		mark_buffer_clean(journal->j_sb_buffer);
	else if (!(ctx->options & E2F_OPT_READONLY)) {
		jsb = journal->j_superblock;
		jsb->s_sequence = htonl(journal->j_tail_sequence);
		if (reset)
			jsb->s_start = 0; /* this marks the journal as empty */
		e2fsck_journal_sb_csum_set(journal, jsb);
		mark_buffer_dirty(journal->j_sb_buffer);
	}
	brelse(journal->j_sb_buffer);

	if (ctx->journal_io) {
		if (ctx->fs && ctx->fs->io != ctx->journal_io)
			io_channel_close(ctx->journal_io);
		ctx->journal_io = 0;
	}

#ifndef USE_INODE_IO
	if (journal->j_inode)
		ext2fs_free_mem(&journal->j_inode);
#endif
	if (journal->j_fs_dev)
		ext2fs_free_mem(&journal->j_fs_dev);
	ext2fs_free_mem(&journal);
}

/*
 * This function makes sure that the superblock fields regarding the
 * journal are consistent.
 */
errcode_t e2fsck_check_ext3_journal(e2fsck_t ctx)
{
	struct ext2_super_block *sb = ctx->fs->super;
	journal_t *journal;
	int recover = ext2fs_has_feature_journal_needs_recovery(ctx->fs->super);
	struct problem_context pctx;
	problem_t problem;
	int reset = 0, force_fsck = 0;
	errcode_t retval;

	/* If we don't have any journal features, don't do anything more */
	if (!ext2fs_has_feature_journal(sb) &&
	    !recover && sb->s_journal_inum == 0 && sb->s_journal_dev == 0 &&
	    uuid_is_null(sb->s_journal_uuid))
 		return 0;

	clear_problem_context(&pctx);
	pctx.num = sb->s_journal_inum;

	retval = e2fsck_get_journal(ctx, &journal);
	if (retval) {
		if ((retval == EXT2_ET_BAD_INODE_NUM) ||
		    (retval == EXT2_ET_BAD_BLOCK_NUM) ||
		    (retval == EXT2_ET_JOURNAL_TOO_SMALL) ||
		    (retval == EXT2_ET_NO_JOURNAL))
			return e2fsck_journal_fix_bad_inode(ctx, &pctx);
		return retval;
	}

	retval = e2fsck_journal_load(journal);
	if (retval) {
		if ((retval == EXT2_ET_CORRUPT_JOURNAL_SB) ||
		    ((retval == EXT2_ET_UNSUPP_FEATURE) &&
		    (!fix_problem(ctx, PR_0_JOURNAL_UNSUPP_INCOMPAT,
				  &pctx))) ||
		    ((retval == EXT2_ET_RO_UNSUPP_FEATURE) &&
		    (!fix_problem(ctx, PR_0_JOURNAL_UNSUPP_ROCOMPAT,
				  &pctx))) ||
		    ((retval == EXT2_ET_JOURNAL_UNSUPP_VERSION) &&
		    (!fix_problem(ctx, PR_0_JOURNAL_UNSUPP_VERSION, &pctx))))
			retval = e2fsck_journal_fix_corrupt_super(ctx, journal,
								  &pctx);
		e2fsck_journal_release(ctx, journal, 0, 1);
		return retval;
	}

	/*
	 * We want to make the flags consistent here.  We will not leave with
	 * needs_recovery set but has_journal clear.  We can't get in a loop
	 * with -y, -n, or -p, only if a user isn't making up their mind.
	 */
no_has_journal:
	if (!ext2fs_has_feature_journal(sb)) {
		recover = ext2fs_has_feature_journal_needs_recovery(sb);
		if (fix_problem(ctx, PR_0_JOURNAL_HAS_JOURNAL, &pctx)) {
			if (recover &&
			    !fix_problem(ctx, PR_0_JOURNAL_RECOVER_SET, &pctx))
				goto no_has_journal;
			/*
			 * Need a full fsck if we are releasing a
			 * journal stored on a reserved inode.
			 */
			force_fsck = recover ||
				(sb->s_journal_inum < EXT2_FIRST_INODE(sb));
			/* Clear all of the journal fields */
			sb->s_journal_inum = 0;
			sb->s_journal_dev = 0;
			memset(sb->s_journal_uuid, 0,
			       sizeof(sb->s_journal_uuid));
			e2fsck_clear_recover(ctx, force_fsck);
		} else if (!(ctx->options & E2F_OPT_READONLY)) {
			ext2fs_set_feature_journal(sb);
			ctx->fs->flags &= ~EXT2_FLAG_MASTER_SB_ONLY;
			ext2fs_mark_super_dirty(ctx->fs);
		}
	}

	if (ext2fs_has_feature_journal(sb) &&
	    !ext2fs_has_feature_journal_needs_recovery(sb) &&
	    journal->j_superblock->s_start != 0) {
		/* Print status information */
		fix_problem(ctx, PR_0_JOURNAL_RECOVERY_CLEAR, &pctx);
		if (ctx->superblock)
			problem = PR_0_JOURNAL_RUN_DEFAULT;
		else
			problem = PR_0_JOURNAL_RUN;
		if (fix_problem(ctx, problem, &pctx)) {
			ctx->options |= E2F_OPT_FORCE;
			ext2fs_set_feature_journal_needs_recovery(sb);
			ext2fs_mark_super_dirty(ctx->fs);
		} else if (fix_problem(ctx,
				       PR_0_JOURNAL_RESET_JOURNAL, &pctx)) {
			reset = 1;
			sb->s_state &= ~EXT2_VALID_FS;
			ext2fs_mark_super_dirty(ctx->fs);
		}
		/*
		 * If the user answers no to the above question, we
		 * ignore the fact that journal apparently has data;
		 * accidentally replaying over valid data would be far
		 * worse than skipping a questionable recovery.
		 *
		 * XXX should we abort with a fatal error here?  What
		 * will the ext3 kernel code do if a filesystem with
		 * !NEEDS_RECOVERY but with a non-zero
		 * journal->j_superblock->s_start is mounted?
		 */
	}

	/*
	 * If we don't need to do replay the journal, check to see if
	 * the journal's errno is set; if so, we need to mark the file
	 * system as being corrupt and clear the journal's s_errno.
	 */
	if (!ext2fs_has_feature_journal_needs_recovery(sb) &&
	    journal->j_superblock->s_errno) {
		ctx->fs->super->s_state |= EXT2_ERROR_FS;
		ext2fs_mark_super_dirty(ctx->fs);
		journal->j_superblock->s_errno = 0;
		e2fsck_journal_sb_csum_set(journal, journal->j_superblock);
		mark_buffer_dirty(journal->j_sb_buffer);
	}

	e2fsck_journal_release(ctx, journal, reset, 0);
	return retval;
}

static errcode_t recover_ext3_journal(e2fsck_t ctx)
{
	struct problem_context	pctx;
	journal_t *journal;
	errcode_t retval;

	clear_problem_context(&pctx);

	journal_init_revoke_caches();
	retval = e2fsck_get_journal(ctx, &journal);
	if (retval)
		return retval;

	retval = e2fsck_journal_load(journal);
	if (retval)
		goto errout;

	retval = journal_init_revoke(journal, 1024);
	if (retval)
		goto errout;

	retval = -journal_recover(journal);
	if (retval)
		goto errout;

	if (journal->j_failed_commit) {
		pctx.ino = journal->j_failed_commit;
		fix_problem(ctx, PR_0_JNL_TXN_CORRUPT, &pctx);
		journal->j_superblock->s_errno = -EINVAL;
		mark_buffer_dirty(journal->j_sb_buffer);
	}

	journal->j_tail_sequence = journal->j_transaction_sequence;

errout:
	journal_destroy_revoke(journal);
	journal_destroy_revoke_caches();
	e2fsck_journal_release(ctx, journal, 1, 0);
	return retval;
}

errcode_t e2fsck_run_ext3_journal(e2fsck_t ctx)
{
	io_manager io_ptr = ctx->fs->io->manager;
	int blocksize = ctx->fs->blocksize;
	errcode_t	retval, recover_retval;
	io_stats	stats = 0;
	unsigned long long kbytes_written = 0;

	printf(_("%s: recovering journal\n"), ctx->device_name);
	if (ctx->options & E2F_OPT_READONLY) {
		printf(_("%s: won't do journal recovery while read-only\n"),
		       ctx->device_name);
		return EXT2_ET_FILE_RO;
	}

	if (ctx->fs->flags & EXT2_FLAG_DIRTY)
		ext2fs_flush(ctx->fs);	/* Force out any modifications */

	recover_retval = recover_ext3_journal(ctx);

	/*
	 * Reload the filesystem context to get up-to-date data from disk
	 * because journal recovery will change the filesystem under us.
	 */
	if (ctx->fs->super->s_kbytes_written &&
	    ctx->fs->io->manager->get_stats)
		ctx->fs->io->manager->get_stats(ctx->fs->io, &stats);
	if (stats && stats->bytes_written)
		kbytes_written = stats->bytes_written >> 10;

	ext2fs_mmp_stop(ctx->fs);
	ext2fs_free(ctx->fs);
	retval = ext2fs_open(ctx->filesystem_name, ctx->openfs_flags,
			     ctx->superblock, blocksize, io_ptr,
			     &ctx->fs);
	if (retval) {
		com_err(ctx->program_name, retval,
			_("while trying to re-open %s"),
			ctx->device_name);
		fatal_error(ctx, 0);
	}
	ctx->fs->priv_data = ctx;
	ctx->fs->now = ctx->now;
	ctx->fs->flags |= EXT2_FLAG_MASTER_SB_ONLY;
	ctx->fs->super->s_kbytes_written += kbytes_written;

	/* Set the superblock flags */
	e2fsck_clear_recover(ctx, recover_retval != 0);

	/*
	 * Do one last sanity check, and propagate journal->s_errno to
	 * the EXT2_ERROR_FS flag in the fs superblock if needed.
	 */
	retval = e2fsck_check_ext3_journal(ctx);
	return retval ? retval : recover_retval;
}

/*
 * This function will move the journal inode from a visible file in
 * the filesystem directory hierarchy to the reserved inode if necessary.
 */
static const char * const journal_names[] = {
	".journal", "journal", ".journal.dat", "journal.dat", 0 };

void e2fsck_move_ext3_journal(e2fsck_t ctx)
{
	struct ext2_super_block *sb = ctx->fs->super;
	struct problem_context	pctx;
	struct ext2_inode 	inode;
	ext2_filsys		fs = ctx->fs;
	ext2_ino_t		ino;
	errcode_t		retval;
	const char * const *	cpp;
	dgrp_t			group;
	int			mount_flags;

	clear_problem_context(&pctx);

	/*
	 * If the filesystem is opened read-only, or there is no
	 * journal, then do nothing.
	 */
	if ((ctx->options & E2F_OPT_READONLY) ||
	    (sb->s_journal_inum == 0) ||
	    !ext2fs_has_feature_journal(sb))
		return;

	/*
	 * Read in the journal inode
	 */
	if (ext2fs_read_inode(fs, sb->s_journal_inum, &inode) != 0)
		return;

	/*
	 * If it's necessary to backup the journal inode, do so.
	 */
	if ((sb->s_jnl_backup_type == 0) ||
	    ((sb->s_jnl_backup_type == EXT3_JNL_BACKUP_BLOCKS) &&
	     memcmp(inode.i_block, sb->s_jnl_blocks, EXT2_N_BLOCKS*4))) {
		if (fix_problem(ctx, PR_0_BACKUP_JNL, &pctx)) {
			memcpy(sb->s_jnl_blocks, inode.i_block,
			       EXT2_N_BLOCKS*4);
			sb->s_jnl_blocks[15] = inode.i_size_high;
			sb->s_jnl_blocks[16] = inode.i_size;
			sb->s_jnl_backup_type = EXT3_JNL_BACKUP_BLOCKS;
			ext2fs_mark_super_dirty(fs);
			fs->flags &= ~EXT2_FLAG_MASTER_SB_ONLY;
		}
	}

	/*
	 * If the journal is already the hidden inode, then do nothing
	 */
	if (sb->s_journal_inum == EXT2_JOURNAL_INO)
		return;

	/*
	 * The journal inode had better have only one link and not be readable.
	 */
	if (inode.i_links_count != 1)
		return;

	/*
	 * If the filesystem is mounted, or we can't tell whether
	 * or not it's mounted, do nothing.
	 */
	retval = ext2fs_check_if_mounted(ctx->filesystem_name, &mount_flags);
	if (retval || (mount_flags & EXT2_MF_MOUNTED))
		return;

	/*
	 * If we can't find the name of the journal inode, then do
	 * nothing.
	 */
	for (cpp = journal_names; *cpp; cpp++) {
		retval = ext2fs_lookup(fs, EXT2_ROOT_INO, *cpp,
				       strlen(*cpp), 0, &ino);
		if ((retval == 0) && (ino == sb->s_journal_inum))
			break;
	}
	if (*cpp == 0)
		return;

	/* We need the inode bitmap to be loaded */
	retval = ext2fs_read_bitmaps(fs);
	if (retval)
		return;

	pctx.str = *cpp;
	if (!fix_problem(ctx, PR_0_MOVE_JOURNAL, &pctx))
		return;

	/*
	 * OK, we've done all the checks, let's actually move the
	 * journal inode.  Errors at this point mean we need to force
	 * an ext2 filesystem check.
	 */
	if ((retval = ext2fs_unlink(fs, EXT2_ROOT_INO, *cpp, ino, 0)) != 0)
		goto err_out;
	if ((retval = ext2fs_write_inode(fs, EXT2_JOURNAL_INO, &inode)) != 0)
		goto err_out;
	sb->s_journal_inum = EXT2_JOURNAL_INO;
	ext2fs_mark_super_dirty(fs);
	fs->flags &= ~EXT2_FLAG_MASTER_SB_ONLY;
	inode.i_links_count = 0;
	inode.i_dtime = ctx->now;
	if ((retval = ext2fs_write_inode(fs, ino, &inode)) != 0)
		goto err_out;

	group = ext2fs_group_of_ino(fs, ino);
	ext2fs_unmark_inode_bitmap2(fs->inode_map, ino);
	ext2fs_mark_ib_dirty(fs);
	ext2fs_bg_free_inodes_count_set(fs, group, ext2fs_bg_free_inodes_count(fs, group) + 1);
	ext2fs_group_desc_csum_set(fs, group);
	fs->super->s_free_inodes_count++;
	return;

err_out:
	pctx.errcode = retval;
	fix_problem(ctx, PR_0_ERR_MOVE_JOURNAL, &pctx);
	fs->super->s_state &= ~EXT2_VALID_FS;
	ext2fs_mark_super_dirty(fs);
	return;
}

/*
 * This function makes sure the superblock hint for the external
 * journal is correct.
 */
int e2fsck_fix_ext3_journal_hint(e2fsck_t ctx)
{
	struct ext2_super_block *sb = ctx->fs->super;
	struct problem_context pctx;
	char uuid[37], *journal_name;
	struct stat st;

	if (!ext2fs_has_feature_journal(sb) ||
	    uuid_is_null(sb->s_journal_uuid))
 		return 0;

	uuid_unparse(sb->s_journal_uuid, uuid);
	journal_name = blkid_get_devname(ctx->blkid, "UUID", uuid);
	if (!journal_name)
		return 0;

	if (stat(journal_name, &st) < 0) {
		free(journal_name);
		return 0;
	}

	if (st.st_rdev != sb->s_journal_dev) {
		clear_problem_context(&pctx);
		pctx.num = st.st_rdev;
		if (fix_problem(ctx, PR_0_EXTERNAL_JOURNAL_HINT, &pctx)) {
			sb->s_journal_dev = st.st_rdev;
			ext2fs_mark_super_dirty(ctx->fs);
		}
	}

	free(journal_name);
	return 0;
}
