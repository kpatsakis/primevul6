static int selinux_inode_rename(struct inode *old_inode, struct dentry *old_dentry,
				struct inode *new_inode, struct dentry *new_dentry)
{
	return may_rename(old_inode, old_dentry, new_inode, new_dentry);
}