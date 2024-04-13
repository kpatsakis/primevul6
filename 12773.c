static void selinux_d_instantiate(struct dentry *dentry, struct inode *inode)
{
	if (inode)
		inode_doinit_with_dentry(inode, dentry);
}