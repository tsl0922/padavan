/*
 * pass1b.c --- Pass #1b of e2fsck
 *
 * This file contains pass1B, pass1C, and pass1D of e2fsck.  They are
 * only invoked if pass 1 discovered blocks which are in use by more
 * than one inode.
 *
 * Pass1B scans the data blocks of all the inodes again, generating a
 * complete list of duplicate blocks and which inodes have claimed
 * them.
 *
 * Pass1C does a tree-traversal of the filesystem, to determine the
 * parent directories of these inodes.  This step is necessary so that
 * e2fsck can print out the pathnames of affected inodes.
 *
 * Pass1D is a reconciliation pass.  For each inode with duplicate
 * blocks, the user is prompted if s/he would like to clone the file
 * (so that the file gets a fresh copy of the duplicated blocks) or
 * simply to delete the file.
 *
 * Copyright (C) 1993, 1994, 1995, 1996, 1997 Theodore Ts'o.
 *
 * %Begin-Header%
 * This file may be redistributed under the terms of the GNU Public
 * License.
 * %End-Header%
 *
 */

#include "config.h"
#include <time.h>
#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif

#ifdef HAVE_INTTYPES_H
#include <inttypes.h>
#endif

#ifndef HAVE_INTPTR_T
typedef long intptr_t;
#endif

/* Needed for architectures where sizeof(int) != sizeof(void *) */
#define INT_TO_VOIDPTR(val)  ((void *)(intptr_t)(val))
#define VOIDPTR_TO_INT(ptr)  ((int)(intptr_t)(ptr))

#include <et/com_err.h>
#include "e2fsck.h"

#include "problem.h"
#include "support/dict.h"

/* Define an extension to the ext2 library's block count information */
#define BLOCK_COUNT_EXTATTR	(-5)

struct cluster_el {
	blk64_t	cluster;
	struct cluster_el *next;
};

struct inode_el {
	ext2_ino_t	inode;
	struct inode_el *next;
};

struct dup_cluster {
	int		num_bad;
	struct inode_el *inode_list;
};

/*
 * This structure stores information about a particular inode which
 * is sharing blocks with other inodes.  This information is collected
 * to display to the user, so that the user knows what files he or she
 * is dealing with, when trying to decide how to resolve the conflict
 * of multiply-claimed blocks.
 */
struct dup_inode {
	ext2_ino_t		dir;
	int			num_dupblocks;
	struct ext2_inode_large	inode;
	struct cluster_el	*cluster_list;
};

static int process_pass1b_block(ext2_filsys fs, blk64_t	*blocknr,
				e2_blkcnt_t blockcnt, blk64_t ref_blk,
				int ref_offset, void *priv_data);
static void delete_file(e2fsck_t ctx, ext2_ino_t ino,
			struct dup_inode *dp, char *block_buf);
static errcode_t clone_file(e2fsck_t ctx, ext2_ino_t ino,
			    struct dup_inode *dp, char* block_buf);
static int check_if_fs_block(e2fsck_t ctx, blk64_t test_block);
static int check_if_fs_cluster(e2fsck_t ctx, blk64_t cluster);

static void pass1b(e2fsck_t ctx, char *block_buf);
static void pass1c(e2fsck_t ctx, char *block_buf);
static void pass1d(e2fsck_t ctx, char *block_buf);

static int dup_inode_count = 0;
static int dup_inode_founddir = 0;

static dict_t clstr_dict, ino_dict;

static ext2fs_inode_bitmap inode_dup_map;

static int dict_int_cmp(const void *cmp_ctx EXT2FS_ATTR((unused)),
			const void *a, const void *b)
{
	intptr_t	ia, ib;

	ia = (intptr_t)a;
	ib = (intptr_t)b;

	return (ia-ib);
}

/*
 * Add a duplicate block record
 */
static void add_dupe(e2fsck_t ctx, ext2_ino_t ino, blk64_t cluster,
		     struct ext2_inode_large *inode)
{
	dnode_t	*n;
	struct dup_cluster	*db;
	struct dup_inode	*di;
	struct cluster_el	*cluster_el;
	struct inode_el 	*ino_el;

	n = dict_lookup(&clstr_dict, INT_TO_VOIDPTR(cluster));
	if (n)
		db = (struct dup_cluster *) dnode_get(n);
	else {
		db = (struct dup_cluster *) e2fsck_allocate_memory(ctx,
			sizeof(struct dup_cluster), "duplicate cluster header");
		db->num_bad = 0;
		db->inode_list = 0;
		dict_alloc_insert(&clstr_dict, INT_TO_VOIDPTR(cluster), db);
	}
	ino_el = (struct inode_el *) e2fsck_allocate_memory(ctx,
			 sizeof(struct inode_el), "inode element");
	ino_el->inode = ino;
	ino_el->next = db->inode_list;
	db->inode_list = ino_el;
	db->num_bad++;

	n = dict_lookup(&ino_dict, INT_TO_VOIDPTR(ino));
	if (n)
		di = (struct dup_inode *) dnode_get(n);
	else {
		di = (struct dup_inode *) e2fsck_allocate_memory(ctx,
			 sizeof(struct dup_inode), "duplicate inode header");
		if (ino == EXT2_ROOT_INO) {
			di->dir = EXT2_ROOT_INO;
			dup_inode_founddir++;
		} else
			di->dir = 0;

		di->num_dupblocks = 0;
		di->cluster_list = 0;
		di->inode = *inode;
		dict_alloc_insert(&ino_dict, INT_TO_VOIDPTR(ino), di);
	}
	cluster_el = (struct cluster_el *) e2fsck_allocate_memory(ctx,
			 sizeof(struct cluster_el), "cluster element");
	cluster_el->cluster = cluster;
	cluster_el->next = di->cluster_list;
	di->cluster_list = cluster_el;
	di->num_dupblocks++;
}

/*
 * Free a duplicate inode record
 */
static void inode_dnode_free(dnode_t *node,
			     void *context EXT2FS_ATTR((unused)))
{
	struct dup_inode	*di;
	struct cluster_el		*p, *next;

	di = (struct dup_inode *) dnode_get(node);
	for (p = di->cluster_list; p; p = next) {
		next = p->next;
		ext2fs_free_mem(&p);
	}
	ext2fs_free_mem(&di);
	ext2fs_free_mem(&node);
}

/*
 * Free a duplicate cluster record
 */
static void cluster_dnode_free(dnode_t *node,
			       void *context EXT2FS_ATTR((unused)))
{
	struct dup_cluster	*dc;
	struct inode_el		*p, *next;

	dc = (struct dup_cluster *) dnode_get(node);
	for (p = dc->inode_list; p; p = next) {
		next = p->next;
		ext2fs_free_mem(&p);
	}
	ext2fs_free_mem(&dc);
	ext2fs_free_mem(&node);
}


/*
 * Main procedure for handling duplicate blocks
 */
void e2fsck_pass1_dupblocks(e2fsck_t ctx, char *block_buf)
{
	ext2_filsys 		fs = ctx->fs;
	struct problem_context	pctx;
#ifdef RESOURCE_TRACK
	struct resource_track	rtrack;
#endif

	clear_problem_context(&pctx);

	pctx.errcode = e2fsck_allocate_inode_bitmap(fs,
			_("multiply claimed inode map"),
			EXT2FS_BMAP64_RBTREE, "inode_dup_map",
			&inode_dup_map);
	if (pctx.errcode) {
		fix_problem(ctx, PR_1B_ALLOCATE_IBITMAP_ERROR, &pctx);
		ctx->flags |= E2F_FLAG_ABORT;
		return;
	}

	dict_init(&ino_dict, DICTCOUNT_T_MAX, dict_int_cmp);
	dict_init(&clstr_dict, DICTCOUNT_T_MAX, dict_int_cmp);
	dict_set_allocator(&ino_dict, NULL, inode_dnode_free, NULL);
	dict_set_allocator(&clstr_dict, NULL, cluster_dnode_free, NULL);

	init_resource_track(&rtrack, ctx->fs->io);
	pass1b(ctx, block_buf);
	print_resource_track(ctx, "Pass 1b", &rtrack, ctx->fs->io);

	init_resource_track(&rtrack, ctx->fs->io);
	pass1c(ctx, block_buf);
	print_resource_track(ctx, "Pass 1c", &rtrack, ctx->fs->io);

	init_resource_track(&rtrack, ctx->fs->io);
	pass1d(ctx, block_buf);
	print_resource_track(ctx, "Pass 1d", &rtrack, ctx->fs->io);

	if (ext2fs_has_feature_shared_blocks(ctx->fs->super) &&
	    (ctx->options & E2F_OPT_UNSHARE_BLOCKS)) {
		/*
		 * If we successfully managed to unshare all blocks, unset the
		 * shared block feature.
		 */
		blk64_t next;
		int result = ext2fs_find_first_set_block_bitmap2(
			ctx->block_dup_map,
			ctx->fs->super->s_first_data_block,
			ext2fs_blocks_count(ctx->fs->super) - 1,
			&next);
		if (result == ENOENT && !(ctx->options & E2F_OPT_NO)) {
			ext2fs_clear_feature_shared_blocks(ctx->fs->super);
			ext2fs_mark_super_dirty(ctx->fs);
		}
	}

	/*
	 * Time to free all of the accumulated data structures that we
	 * don't need anymore.
	 */
	dict_free_nodes(&ino_dict);
	dict_free_nodes(&clstr_dict);
	ext2fs_free_inode_bitmap(inode_dup_map);
}

/*
 * Scan the inodes looking for inodes that contain duplicate blocks.
 */
struct process_block_struct {
	e2fsck_t	ctx;
	ext2_ino_t	ino;
	int		dup_blocks;
	blk64_t		cur_cluster, phys_cluster;
	blk64_t		last_blk;
	struct ext2_inode_large *inode;
	struct problem_context *pctx;
};

static void pass1b(e2fsck_t ctx, char *block_buf)
{
	ext2_filsys fs = ctx->fs;
	ext2_ino_t ino = 0;
	struct ext2_inode_large inode;
	ext2_inode_scan	scan;
	struct process_block_struct pb;
	struct problem_context pctx;
	problem_t op;

	clear_problem_context(&pctx);

	if (!(ctx->options & E2F_OPT_PREEN))
		fix_problem(ctx, PR_1B_PASS_HEADER, &pctx);
	pctx.errcode = ext2fs_open_inode_scan(fs, ctx->inode_buffer_blocks,
					      &scan);
	if (pctx.errcode) {
		fix_problem(ctx, PR_1B_ISCAN_ERROR, &pctx);
		ctx->flags |= E2F_FLAG_ABORT;
		return;
	}
	ctx->stashed_inode = EXT2_INODE(&inode);
	pb.ctx = ctx;
	pb.pctx = &pctx;
	pctx.str = "pass1b";
	while (1) {
		if (ino % (fs->super->s_inodes_per_group * 4) == 1) {
			if (e2fsck_mmp_update(fs))
				fatal_error(ctx, 0);
		}
		pctx.errcode = ext2fs_get_next_inode_full(scan, &ino,
				EXT2_INODE(&inode), sizeof(inode));
		if (pctx.errcode == EXT2_ET_BAD_BLOCK_IN_INODE_TABLE)
			continue;
		if (pctx.errcode) {
			pctx.ino = ino;
			fix_problem(ctx, PR_1B_ISCAN_ERROR, &pctx);
			ctx->flags |= E2F_FLAG_ABORT;
			return;
		}
		if (!ino)
			break;
		pctx.ino = ctx->stashed_ino = ino;
		if ((ino != EXT2_BAD_INO) &&
		    !ext2fs_test_inode_bitmap2(ctx->inode_used_map, ino))
			continue;

		pb.ino = ino;
		pb.dup_blocks = 0;
		pb.inode = &inode;
		pb.cur_cluster = ~0;
		pb.phys_cluster = ~0;
		pb.last_blk = 0;
		pb.pctx->blk = pb.pctx->blk2 = 0;

		if (ext2fs_inode_has_valid_blocks2(fs, EXT2_INODE(&inode)) ||
		    (ino == EXT2_BAD_INO))
			pctx.errcode = ext2fs_block_iterate3(fs, ino,
					     BLOCK_FLAG_READ_ONLY, block_buf,
					     process_pass1b_block, &pb);
		/* If the feature is not set, attrs will be cleared later anyway */
		if (ext2fs_has_feature_xattr(fs->super) &&
		    ext2fs_file_acl_block(fs, EXT2_INODE(&inode))) {
			blk64_t blk = ext2fs_file_acl_block(fs, EXT2_INODE(&inode));
			process_pass1b_block(fs, &blk,
					     BLOCK_COUNT_EXTATTR, 0, 0, &pb);
			ext2fs_file_acl_block_set(fs, EXT2_INODE(&inode), blk);
		}
		if (pb.dup_blocks) {
			if (ino != EXT2_BAD_INO) {
				op = pctx.blk == pctx.blk2 ?
					PR_1B_DUP_BLOCK : PR_1B_DUP_RANGE;
				fix_problem(ctx, op, pb.pctx);
			}
			end_problem_latch(ctx, PR_LATCH_DBLOCK);
			if (ino >= EXT2_FIRST_INODE(fs->super) ||
			    ino == EXT2_ROOT_INO)
				dup_inode_count++;
		}
		if (pctx.errcode)
			fix_problem(ctx, PR_1B_BLOCK_ITERATE, &pctx);
	}
	ext2fs_close_inode_scan(scan);
	e2fsck_use_inode_shortcuts(ctx, 0);
}

static int process_pass1b_block(ext2_filsys fs EXT2FS_ATTR((unused)),
				blk64_t	*block_nr,
				e2_blkcnt_t blockcnt,
				blk64_t ref_blk EXT2FS_ATTR((unused)),
				int ref_offset EXT2FS_ATTR((unused)),
				void *priv_data)
{
	struct process_block_struct *p;
	e2fsck_t ctx;
	blk64_t	lc, pc;
	problem_t op;

	if (*block_nr == 0)
		return 0;
	p = (struct process_block_struct *) priv_data;
	ctx = p->ctx;
	lc = EXT2FS_B2C(fs, blockcnt);
	pc = EXT2FS_B2C(fs, *block_nr);

	if (!ext2fs_test_block_bitmap2(ctx->block_dup_map, *block_nr))
		goto finish;

	/* OK, this is a duplicate block */
	if (p->ino != EXT2_BAD_INO) {
		if (p->last_blk + 1 != *block_nr) {
			if (p->last_blk) {
				op = p->pctx->blk == p->pctx->blk2 ?
						PR_1B_DUP_BLOCK :
						PR_1B_DUP_RANGE;
				fix_problem(ctx, op, p->pctx);
			}
			p->pctx->blk = *block_nr;
		}
		p->pctx->blk2 = *block_nr;
		p->last_blk = *block_nr;
	}
	p->dup_blocks++;
	ext2fs_mark_inode_bitmap2(inode_dup_map, p->ino);

	/*
	 * Qualifications for submitting a block for duplicate processing:
	 * It's an extent/indirect block (and has a negative logical offset);
	 * we've crossed a logical cluster boundary; or the physical cluster
	 * suddenly changed, which indicates that blocks in a logical cluster
	 * are mapped to multiple physical clusters.
	 */
	if (blockcnt < 0 || lc != p->cur_cluster || pc != p->phys_cluster)
		add_dupe(ctx, p->ino, EXT2FS_B2C(fs, *block_nr), p->inode);

finish:
	p->cur_cluster = lc;
	p->phys_cluster = pc;
	return 0;
}

/*
 * Pass 1c: Scan directories for inodes with duplicate blocks.  This
 * is used so that we can print pathnames when prompting the user for
 * what to do.
 */
struct search_dir_struct {
	int		count;
	ext2_ino_t	first_inode;
	ext2_ino_t	max_inode;
};

static int search_dirent_proc(ext2_ino_t dir, int entry,
			      struct ext2_dir_entry *dirent,
			      int offset EXT2FS_ATTR((unused)),
			      int blocksize EXT2FS_ATTR((unused)),
			      char *buf EXT2FS_ATTR((unused)),
			      void *priv_data)
{
	struct search_dir_struct *sd;
	struct dup_inode	*p;
	dnode_t			*n;

	sd = (struct search_dir_struct *) priv_data;

	if (dirent->inode > sd->max_inode)
		/* Should abort this inode, but not everything */
		return 0;

	if ((dirent->inode < sd->first_inode) || (entry < DIRENT_OTHER_FILE) ||
	    !ext2fs_test_inode_bitmap2(inode_dup_map, dirent->inode))
		return 0;

	n = dict_lookup(&ino_dict, INT_TO_VOIDPTR(dirent->inode));
	if (!n)
		return 0;
	p = (struct dup_inode *) dnode_get(n);
	if (!p->dir) {
		p->dir = dir;
		sd->count--;
	}

	return(sd->count ? 0 : DIRENT_ABORT);
}


static void pass1c(e2fsck_t ctx, char *block_buf)
{
	ext2_filsys fs = ctx->fs;
	struct search_dir_struct sd;
	struct problem_context pctx;

	clear_problem_context(&pctx);

	if (!(ctx->options & E2F_OPT_PREEN))
		fix_problem(ctx, PR_1C_PASS_HEADER, &pctx);

	/*
	 * Search through all directories to translate inodes to names
	 * (by searching for the containing directory for that inode.)
	 */
	sd.count = dup_inode_count - dup_inode_founddir;
	sd.first_inode = EXT2_FIRST_INODE(fs->super);
	sd.max_inode = fs->super->s_inodes_count;
	ext2fs_dblist_dir_iterate(fs->dblist, 0, block_buf,
				  search_dirent_proc, &sd);
}

static void pass1d(e2fsck_t ctx, char *block_buf)
{
	ext2_filsys fs = ctx->fs;
	struct dup_inode	*p, *t;
	struct dup_cluster	*q;
	ext2_ino_t		*shared, ino;
	int	shared_len;
	int	i;
	int	file_ok;
	int	meta_data = 0;
	struct problem_context pctx;
	dnode_t	*n, *m;
	struct cluster_el	*s;
	struct inode_el *r;

	clear_problem_context(&pctx);

	if (!(ctx->options & E2F_OPT_PREEN))
		fix_problem(ctx, PR_1D_PASS_HEADER, &pctx);
	e2fsck_read_bitmaps(ctx);

	pctx.num = dup_inode_count; /* dict_count(&ino_dict); */
	fix_problem(ctx, PR_1D_NUM_DUP_INODES, &pctx);
	shared = (ext2_ino_t *) e2fsck_allocate_memory(ctx,
				sizeof(ext2_ino_t) * dict_count(&ino_dict),
				"Shared inode list");
	for (n = dict_first(&ino_dict); n; n = dict_next(&ino_dict, n)) {
		p = (struct dup_inode *) dnode_get(n);
		shared_len = 0;
		file_ok = 1;
		ino = (ext2_ino_t)VOIDPTR_TO_INT(dnode_getkey(n));
		if (ino == EXT2_BAD_INO || ino == EXT2_RESIZE_INO)
			continue;

		/*
		 * Find all of the inodes which share blocks with this
		 * one.  First we find all of the duplicate blocks
		 * belonging to this inode, and then search each block
		 * get the list of inodes, and merge them together.
		 */
		for (s = p->cluster_list; s; s = s->next) {
			m = dict_lookup(&clstr_dict,
					INT_TO_VOIDPTR(s->cluster));
			if (!m)
				continue; /* Should never happen... */
			q = (struct dup_cluster *) dnode_get(m);
			if (q->num_bad > 1)
				file_ok = 0;
			if (check_if_fs_cluster(ctx, s->cluster)) {
				file_ok = 0;
				meta_data = 1;
			}

			/*
			 * Add all inodes used by this block to the
			 * shared[] --- which is a unique list, so
			 * if an inode is already in shared[], don't
			 * add it again.
			 */
			for (r = q->inode_list; r; r = r->next) {
				if (r->inode == ino)
					continue;
				for (i = 0; i < shared_len; i++)
					if (shared[i] == r->inode)
						break;
				if (i == shared_len) {
					shared[shared_len++] = r->inode;
				}
			}
		}

		/*
		 * Report the inode that we are working on
		 */
		pctx.inode = EXT2_INODE(&p->inode);
		pctx.ino = ino;
		pctx.dir = p->dir;
		pctx.blkcount = p->num_dupblocks;
		pctx.num = meta_data ? shared_len+1 : shared_len;
		fix_problem(ctx, PR_1D_DUP_FILE, &pctx);
		pctx.blkcount = 0;
		pctx.num = 0;

		if (meta_data)
			fix_problem(ctx, PR_1D_SHARE_METADATA, &pctx);

		for (i = 0; i < shared_len; i++) {
			m = dict_lookup(&ino_dict, INT_TO_VOIDPTR(shared[i]));
			if (!m)
				continue; /* should never happen */
			t = (struct dup_inode *) dnode_get(m);
			/*
			 * Report the inode that we are sharing with
			 */
			pctx.inode = EXT2_INODE(&t->inode);
			pctx.ino = shared[i];
			pctx.dir = t->dir;
			fix_problem(ctx, PR_1D_DUP_FILE_LIST, &pctx);
		}
		/*
		 * Even if the file shares blocks with itself, we still need to
		 * clone the blocks.
		 */
		if (file_ok && (meta_data ? shared_len+1 : shared_len) != 0) {
			fix_problem(ctx, PR_1D_DUP_BLOCKS_DEALT, &pctx);
			continue;
		}
		if ((ctx->options & E2F_OPT_UNSHARE_BLOCKS) ||
                    fix_problem(ctx, PR_1D_CLONE_QUESTION, &pctx)) {
			pctx.errcode = clone_file(ctx, ino, p, block_buf);
			if (pctx.errcode)
				fix_problem(ctx, PR_1D_CLONE_ERROR, &pctx);
			else
				continue;
		}
		/*
		 * Note: When unsharing blocks, we don't prompt to delete
		 * files. If the clone operation fails than the unshare
		 * operation should fail too.
		 */
		if (!(ctx->options & E2F_OPT_UNSHARE_BLOCKS) &&
                    fix_problem(ctx, PR_1D_DELETE_QUESTION, &pctx))
			delete_file(ctx, ino, p, block_buf);
		else
			ext2fs_unmark_valid(fs);
	}
	ext2fs_free_mem(&shared);
}

/*
 * Drop the refcount on the dup_block structure, and clear the entry
 * in the block_dup_map if appropriate.
 */
static void decrement_badcount(e2fsck_t ctx, blk64_t block,
			       struct dup_cluster *p)
{
	p->num_bad--;
	if (p->num_bad <= 0 ||
	    (p->num_bad == 1 && !check_if_fs_block(ctx, block))) {
		if (check_if_fs_cluster(ctx, EXT2FS_B2C(ctx->fs, block)))
			return;
		ext2fs_unmark_block_bitmap2(ctx->block_dup_map, block);
	}
}

static int delete_file_block(ext2_filsys fs,
			     blk64_t	*block_nr,
			     e2_blkcnt_t blockcnt,
			     blk64_t ref_block EXT2FS_ATTR((unused)),
			     int ref_offset EXT2FS_ATTR((unused)),
			     void *priv_data)
{
	struct process_block_struct *pb;
	struct dup_cluster *p;
	dnode_t	*n;
	e2fsck_t ctx;
	blk64_t c, lc;

	pb = (struct process_block_struct *) priv_data;
	ctx = pb->ctx;

	if (*block_nr == 0)
		return 0;

	c = EXT2FS_B2C(fs, *block_nr);
	lc = EXT2FS_B2C(fs, blockcnt);
	if (ext2fs_test_block_bitmap2(ctx->block_dup_map, *block_nr)) {
		n = dict_lookup(&clstr_dict, INT_TO_VOIDPTR(c));
		if (n) {
			if (lc != pb->cur_cluster) {
				p = (struct dup_cluster *) dnode_get(n);
				decrement_badcount(ctx, *block_nr, p);
				pb->dup_blocks++;
			}
		} else
			com_err("delete_file_block", 0,
			    _("internal error: can't find dup_blk for %llu\n"),
				(unsigned long long) *block_nr);
	} else {
		if ((*block_nr % EXT2FS_CLUSTER_RATIO(ctx->fs)) == 0)
			ext2fs_block_alloc_stats2(fs, *block_nr, -1);
		pb->dup_blocks++;
	}
	pb->cur_cluster = lc;

	return 0;
}

static void delete_file(e2fsck_t ctx, ext2_ino_t ino,
			struct dup_inode *dp, char* block_buf)
{
	ext2_filsys fs = ctx->fs;
	struct process_block_struct pb;
	struct problem_context	pctx;
	unsigned int		count;

	clear_problem_context(&pctx);
	pctx.ino = pb.ino = ino;
	pb.dup_blocks = 0;
	pb.ctx = ctx;
	pctx.str = "delete_file";
	pb.cur_cluster = ~0;

	if (ext2fs_inode_has_valid_blocks2(fs, EXT2_INODE(&dp->inode)))
		pctx.errcode = ext2fs_block_iterate3(fs, ino,
						     BLOCK_FLAG_READ_ONLY,
						     block_buf,
						     delete_file_block, &pb);
	if (pctx.errcode)
		fix_problem(ctx, PR_1B_BLOCK_ITERATE, &pctx);
	if (ctx->inode_bad_map)
		ext2fs_unmark_inode_bitmap2(ctx->inode_bad_map, ino);
	if (ctx->inode_reg_map)
		ext2fs_unmark_inode_bitmap2(ctx->inode_reg_map, ino);
	ext2fs_unmark_inode_bitmap2(ctx->inode_dir_map, ino);
	ext2fs_unmark_inode_bitmap2(ctx->inode_used_map, ino);
	ext2fs_inode_alloc_stats2(fs, ino, -1, LINUX_S_ISDIR(dp->inode.i_mode));
	quota_data_sub(ctx->qctx, &dp->inode, ino,
		       pb.dup_blocks * fs->blocksize);
	quota_data_inodes(ctx->qctx, &dp->inode, ino, -1);

	/* Inode may have changed by block_iterate, so reread it */
	e2fsck_read_inode_full(ctx, ino, EXT2_INODE(&dp->inode),
			       sizeof(dp->inode), "delete_file");
	e2fsck_clear_inode(ctx, ino, EXT2_INODE(&dp->inode), 0, "delete_file");
	if (ext2fs_file_acl_block(fs, EXT2_INODE(&dp->inode)) &&
	    ext2fs_has_feature_xattr(fs->super)) {
		blk64_t file_acl_block = ext2fs_file_acl_block(fs,
						EXT2_INODE(&dp->inode));

		count = 1;
		pctx.errcode = ext2fs_adjust_ea_refcount3(fs, file_acl_block,
					block_buf, -1, &count, ino);
		if (pctx.errcode == EXT2_ET_BAD_EA_BLOCK_NUM) {
			pctx.errcode = 0;
			count = 1;
		}
		if (pctx.errcode) {
			pctx.blk = file_acl_block;
			fix_problem(ctx, PR_1B_ADJ_EA_REFCOUNT, &pctx);
		}
		/*
		 * If the count is zero, then arrange to have the
		 * block deleted.  If the block is in the block_dup_map,
		 * also call delete_file_block since it will take care
		 * of keeping the accounting straight.
		 */
		if ((count == 0) ||
		    ext2fs_test_block_bitmap2(ctx->block_dup_map,
					      file_acl_block)) {
			delete_file_block(fs, &file_acl_block,
					  BLOCK_COUNT_EXTATTR, 0, 0, &pb);
			ext2fs_file_acl_block_set(fs, EXT2_INODE(&dp->inode),
						  file_acl_block);
			quota_data_sub(ctx->qctx, &dp->inode, ino,
				       fs->blocksize);
		}
	}
}

struct clone_struct {
	errcode_t	errcode;
	blk64_t		dup_cluster;
	blk64_t		alloc_block;
	ext2_ino_t	dir, ino;
	char	*buf;
	e2fsck_t ctx;
	struct ext2_inode_large	*inode;

	struct dup_cluster *save_dup_cluster;
	blk64_t save_blocknr;
};

/*
 * Decrement the bad count *after* we've shown that (a) we can allocate a
 * replacement block and (b) remap the file blocks.  Unfortunately, there's no
 * way to find out if the remap succeeded until either the next
 * clone_file_block() call (an error when remapping the block after returning
 * BLOCK_CHANGED will halt the iteration) or after block_iterate() returns.
 * Otherwise, it's possible that we decrease the badcount once in preparation
 * to remap, then the remap fails (either we can't find a replacement block or
 * we have to split the extent tree and can't find a new extent block), so we
 * delete the file, which decreases the badcount again.
 */
static void deferred_dec_badcount(struct clone_struct *cs)
{
	if (!cs->save_dup_cluster)
		return;
	decrement_badcount(cs->ctx, cs->save_blocknr, cs->save_dup_cluster);
	cs->save_dup_cluster = NULL;
}

static int clone_file_block(ext2_filsys fs,
			    blk64_t	*block_nr,
			    e2_blkcnt_t blockcnt,
			    blk64_t ref_block EXT2FS_ATTR((unused)),
			    int ref_offset EXT2FS_ATTR((unused)),
			    void *priv_data)
{
	struct dup_cluster *p = NULL;
	blk64_t	new_block;
	errcode_t	retval;
	struct clone_struct *cs = (struct clone_struct *) priv_data;
	dnode_t *n;
	e2fsck_t ctx;
	blk64_t c;
	int is_meta = 0;
	int should_write = 1;

	ctx = cs->ctx;
	deferred_dec_badcount(cs);

	if (*block_nr == 0)
		return 0;

	if (ext2fs_has_feature_shared_blocks(ctx->fs->super) &&
	    (ctx->options & E2F_OPT_UNSHARE_BLOCKS) &&
	    (ctx->options & E2F_OPT_NO))
		should_write = 0;

	c = EXT2FS_B2C(fs, blockcnt);
	if (check_if_fs_cluster(ctx, EXT2FS_B2C(fs, *block_nr)))
		is_meta = 1;

	if (c == cs->dup_cluster && cs->alloc_block) {
		new_block = cs->alloc_block;
		goto got_block;
	}

	if (ext2fs_test_block_bitmap2(ctx->block_dup_map, *block_nr)) {
		n = dict_lookup(&clstr_dict,
				INT_TO_VOIDPTR(EXT2FS_B2C(fs, *block_nr)));
		if (!n) {
			com_err("clone_file_block", 0,
			    _("internal error: can't find dup_blk for %llu\n"),
				(unsigned long long) *block_nr);
			return 0;
		}

		p = (struct dup_cluster *) dnode_get(n);

		cs->dup_cluster = c;
		/*
		 * Let's try an implied cluster allocation.  If we get the same
		 * cluster back, then we need to find a new block; otherwise,
		 * we're merely fixing the problem of one logical cluster being
		 * mapped to multiple physical clusters.
		 */
		new_block = 0;
		retval = ext2fs_map_cluster_block(fs, cs->ino,
						  EXT2_INODE(cs->inode),
						  blockcnt, &new_block);
		if (retval == 0 && new_block != 0 &&
		    EXT2FS_B2C(ctx->fs, new_block) !=
		    EXT2FS_B2C(ctx->fs, *block_nr))
			goto cluster_alloc_ok;
		retval = ext2fs_new_block2(fs, 0, ctx->block_found_map,
					   &new_block);
		if (retval) {
			cs->errcode = retval;
			return BLOCK_ABORT;
		}
		if (ext2fs_has_feature_shared_blocks(fs->super)) {
			/*
			 * Update the block stats so we don't get a prompt to fix block
			 * counts in the final pass.
			 */
			ext2fs_block_alloc_stats2(fs, new_block, +1);
		}
cluster_alloc_ok:
		cs->alloc_block = new_block;

	got_block:
		new_block &= ~EXT2FS_CLUSTER_MASK(fs);
		new_block += EXT2FS_CLUSTER_MASK(fs) & blockcnt;
		if (cs->dir && (blockcnt >= 0)) {
			retval = ext2fs_set_dir_block2(fs->dblist,
					cs->dir, new_block, blockcnt);
			if (retval) {
				cs->errcode = retval;
				return BLOCK_ABORT;
			}
		}
#if 0
 		printf("Cloning block #%lld from %llu to %llu\n",
		       blockcnt, (unsigned long long) *block_nr,
		       (unsigned long long) new_block);
#endif
		retval = io_channel_read_blk64(fs->io, *block_nr, 1, cs->buf);
		if (retval) {
			cs->errcode = retval;
			return BLOCK_ABORT;
		}
		if (should_write) {
			retval = io_channel_write_blk64(fs->io, new_block, 1, cs->buf);
			if (retval) {
				cs->errcode = retval;
				return BLOCK_ABORT;
			}
		}
		cs->save_dup_cluster = (is_meta ? NULL : p);
		cs->save_blocknr = *block_nr;
		*block_nr = new_block;
		ext2fs_mark_block_bitmap2(ctx->block_found_map, new_block);
		ext2fs_mark_block_bitmap2(fs->block_map, new_block);

		if (!should_write) {
			/* Don't try to change extent information; we want e2fsck to
			 * return success.
			 */
			return 0;
		}
		return BLOCK_CHANGED;
	}
	return 0;
}

static errcode_t clone_file(e2fsck_t ctx, ext2_ino_t ino,
			    struct dup_inode *dp, char* block_buf)
{
	ext2_filsys fs = ctx->fs;
	errcode_t	retval;
	struct clone_struct cs;
	struct problem_context	pctx;
	blk64_t		blk, new_blk;
	dnode_t		*n;
	struct inode_el	*ino_el;
	struct dup_cluster	*dc;
	struct dup_inode	*di;

	clear_problem_context(&pctx);
	cs.errcode = 0;
	cs.dir = 0;
	cs.dup_cluster = ~0;
	cs.alloc_block = 0;
	cs.ctx = ctx;
	cs.ino = ino;
	cs.inode = &dp->inode;
	cs.save_dup_cluster = NULL;
	cs.save_blocknr = 0;
	retval = ext2fs_get_mem(fs->blocksize, &cs.buf);
	if (retval)
		return retval;

	if (ext2fs_test_inode_bitmap2(ctx->inode_dir_map, ino))
		cs.dir = ino;

	pctx.ino = ino;
	pctx.str = "clone_file";
	if (ext2fs_inode_has_valid_blocks2(fs, EXT2_INODE(&dp->inode)))
		pctx.errcode = ext2fs_block_iterate3(fs, ino, 0, block_buf,
						     clone_file_block, &cs);
	deferred_dec_badcount(&cs);
	ext2fs_mark_bb_dirty(fs);
	if (pctx.errcode) {
		fix_problem(ctx, PR_1B_BLOCK_ITERATE, &pctx);
		retval = pctx.errcode;
		goto errout;
	}
	if (cs.errcode) {
		com_err("clone_file", cs.errcode, "%s",
			_("returned from clone_file_block"));
		retval = cs.errcode;
		goto errout;
	}
	/* The inode may have changed on disk, so we have to re-read it */
	e2fsck_read_inode_full(ctx, ino, EXT2_INODE(&dp->inode),
			       sizeof(dp->inode), "clone file EA");
	blk = ext2fs_file_acl_block(fs, EXT2_INODE(&dp->inode));
	new_blk = blk;
	if (blk && (clone_file_block(fs, &new_blk,
				     BLOCK_COUNT_EXTATTR, 0, 0, &cs) ==
		    BLOCK_CHANGED)) {
		ext2fs_file_acl_block_set(fs, EXT2_INODE(&dp->inode), new_blk);
		e2fsck_write_inode_full(ctx, ino, EXT2_INODE(&dp->inode),
					sizeof(dp->inode), "clone file EA");
		/*
		 * If we cloned the EA block, find all other inodes
		 * which referred to that EA block, and modify
		 * them to point to the new EA block.
		 */
		n = dict_lookup(&clstr_dict,
				INT_TO_VOIDPTR(EXT2FS_B2C(fs, blk)));
		if (!n) {
			com_err("clone_file", 0,
				_("internal error: couldn't lookup EA "
				  "block record for %llu"),
				(unsigned long long) blk);
			retval = 0; /* OK to stumble on... */
			goto errout;
		}
		dc = (struct dup_cluster *) dnode_get(n);
		for (ino_el = dc->inode_list; ino_el; ino_el = ino_el->next) {
			if (ino_el->inode == ino)
				continue;
			n = dict_lookup(&ino_dict, INT_TO_VOIDPTR(ino_el->inode));
			if (!n) {
				com_err("clone_file", 0,
					_("internal error: couldn't lookup EA "
					  "inode record for %u"),
					ino_el->inode);
				retval = 0; /* OK to stumble on... */
				goto errout;
			}
			di = (struct dup_inode *) dnode_get(n);
			if (ext2fs_file_acl_block(fs,
					EXT2_INODE(&di->inode)) == blk) {
				ext2fs_file_acl_block_set(fs,
					EXT2_INODE(&di->inode),
					ext2fs_file_acl_block(fs, EXT2_INODE(&dp->inode)));
				e2fsck_write_inode_full(ctx, ino_el->inode,
					EXT2_INODE(&di->inode),
					sizeof(di->inode), "clone file EA");
				decrement_badcount(ctx, blk, dc);
			}
		}
	}
	retval = 0;
errout:
	ext2fs_free_mem(&cs.buf);
	return retval;
}

/*
 * This routine returns 1 if a block overlaps with one of the superblocks,
 * group descriptors, inode bitmaps, or block bitmaps.
 */
static int check_if_fs_block(e2fsck_t ctx, blk64_t test_block)
{
	ext2_filsys fs = ctx->fs;
	blk64_t	first_block;
	dgrp_t	i;

	first_block = fs->super->s_first_data_block;
	for (i = 0; i < fs->group_desc_count; i++) {

		/* Check superblocks/block group descriptors */
		if (ext2fs_bg_has_super(fs, i)) {
			if (test_block >= first_block &&
			    (test_block <= first_block + fs->desc_blocks))
				return 1;
		}

		/* Check the inode table */
		if ((ext2fs_inode_table_loc(fs, i)) &&
		    (test_block >= ext2fs_inode_table_loc(fs, i)) &&
		    (test_block < (ext2fs_inode_table_loc(fs, i) +
				   fs->inode_blocks_per_group)))
			return 1;

		/* Check the bitmap blocks */
		if ((test_block == ext2fs_block_bitmap_loc(fs, i)) ||
		    (test_block == ext2fs_inode_bitmap_loc(fs, i)))
			return 1;

		first_block += fs->super->s_blocks_per_group;
	}
	return 0;
}

/*
 * This routine returns 1 if a cluster overlaps with one of the superblocks,
 * group descriptors, inode bitmaps, or block bitmaps.
 */
static int check_if_fs_cluster(e2fsck_t ctx, blk64_t cluster)
{
	ext2_filsys fs = ctx->fs;
	blk64_t	first_block;
	dgrp_t	i;

	first_block = fs->super->s_first_data_block;
	for (i = 0; i < fs->group_desc_count; i++) {

		/* Check superblocks/block group descriptors */
		if (ext2fs_bg_has_super(fs, i)) {
			if (cluster >= EXT2FS_B2C(fs, first_block) &&
			    (cluster <= EXT2FS_B2C(fs, first_block +
						   fs->desc_blocks)))
				return 1;
		}

		/* Check the inode table */
		if ((ext2fs_inode_table_loc(fs, i)) &&
		    (cluster >= EXT2FS_B2C(fs,
					   ext2fs_inode_table_loc(fs, i))) &&
		    (cluster <= EXT2FS_B2C(fs,
					   ext2fs_inode_table_loc(fs, i) +
					   fs->inode_blocks_per_group - 1)))
			return 1;

		/* Check the bitmap blocks */
		if ((cluster == EXT2FS_B2C(fs,
					   ext2fs_block_bitmap_loc(fs, i))) ||
		    (cluster == EXT2FS_B2C(fs,
					   ext2fs_inode_bitmap_loc(fs, i))))
			return 1;

		first_block += fs->super->s_blocks_per_group;
	}
	return 0;
}
