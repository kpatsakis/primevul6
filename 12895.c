static int selinux_inode_rmdir(struct inode *dir, struct dentry *dentry)
{
	return may_link(dir, dentry, MAY_RMDIR);
}