/*
 * e2fsck.c - a consistency checker for the new extended file system.
 * 
 * Copyright (C) 1993, 1994, 1995, 1996, 1997 Theodore Ts'o.
 *
 * %Begin-Header%
 * This file may be redistributed under the terms of the GNU Public
 * License.
 * %End-Header%
 */

#include <errno.h>

#include "et/com_err.h"

#include "e2fsck.h"
#include "problem.h"

/*
 * This function allocates an e2fsck context 
 */
errcode_t e2fsck_allocate_context(e2fsck_t *ret)
{
	e2fsck_t	context;

	context = malloc(sizeof(struct e2fsck_struct));
	if (!context)
		return ENOMEM;

	memset(context, 0, sizeof(struct e2fsck_struct));

	context->process_inode_size = 256;

	*ret = context;
	return 0;
}

/*
 * This function resets an e2fsck context; it is called when e2fsck
 * needs to be restarted.
 */
errcode_t e2fsck_reset_context(e2fsck_t ctx)
{
	if (ctx->inode_used_map) {
		ext2fs_free_inode_bitmap(ctx->inode_used_map);
		ctx->inode_used_map = 0;
	}
	if (ctx->inode_dir_map) {
		ext2fs_free_inode_bitmap(ctx->inode_dir_map);
		ctx->inode_dir_map = 0;
	}
	if (ctx->block_found_map) {
		ext2fs_free_block_bitmap(ctx->block_found_map);
		ctx->block_found_map = 0;
	}
	if (ctx->inode_link_info) {
		ext2fs_free_icount(ctx->inode_link_info);
		ctx->inode_link_info = 0;
	}
	if (ctx->fs->dblist) {
		ext2fs_free_dblist(ctx->fs->dblist);
		ctx->fs->dblist = 0;
	}
	free_dir_info(ctx->fs);
	if (ctx->block_dup_map) {
		ext2fs_free_block_bitmap(ctx->block_dup_map);
		ctx->block_dup_map = 0;
	}
	if (ctx->inode_bb_map) {
		ext2fs_free_inode_bitmap(ctx->inode_bb_map);
		ctx->inode_bb_map = 0;
	}
	if (ctx->inode_bad_map) {
		ext2fs_free_inode_bitmap(ctx->inode_bad_map);
		ctx->inode_bad_map = 0;
	}

	/*
	 * Clear the array of invalid meta-data flags
	 */
	if (ctx->invalid_inode_bitmap_flag) {
		free(ctx->invalid_inode_bitmap_flag);
		ctx->invalid_inode_bitmap_flag = 0;
	}
	if (ctx->invalid_block_bitmap_flag) {
		free(ctx->invalid_block_bitmap_flag);
		ctx->invalid_block_bitmap_flag = 0;
	}
	if (ctx->invalid_inode_table_flag) {
		free(ctx->invalid_inode_table_flag);
		ctx->invalid_inode_table_flag = 0;
	}

	/* Clear statistic counters */
	ctx->fs_directory_count = 0;
	ctx->fs_regular_count = 0;
	ctx->fs_blockdev_count = 0;
	ctx->fs_chardev_count = 0;
	ctx->fs_links_count = 0;
	ctx->fs_symlinks_count = 0;
	ctx->fs_fast_symlinks_count = 0;
	ctx->fs_fifo_count = 0;
	ctx->fs_total_count = 0;
	ctx->fs_badblocks_count = 0;
	ctx->fs_sockets_count = 0;
	ctx->fs_ind_count = 0;
	ctx->fs_dind_count = 0;
	ctx->fs_tind_count = 0;
	ctx->fs_fragmented = 0;

	/* Reset the superblock to the user's requested value */
	ctx->superblock = ctx->use_superblock;
	
	return 0;
}

void e2fsck_free_context(e2fsck_t ctx)
{
	if (!ctx)
		return;
	
	e2fsck_reset_context(ctx);
	
	free(ctx);
}
