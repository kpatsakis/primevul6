static struct inode *bm_get_inode(struct super_block *sb, int mode)
{
	struct inode * inode = new_inode(sb);

	if (inode) {
		inode->i_mode = mode;
		inode->i_uid = 0;
		inode->i_gid = 0;
		inode->i_blocks = 0;
		inode->i_atime = inode->i_mtime = inode->i_ctime =
			current_fs_time(inode->i_sb);
	}
	return inode;
}