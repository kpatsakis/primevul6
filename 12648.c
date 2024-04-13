static int selinux_inode_unlink(struct inode *dir, struct dentry *dentry)
{
	return may_link(dir, dentry, MAY_UNLINK);
}