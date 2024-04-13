static int selinux_inode_mkdir(struct inode *dir, struct dentry *dentry, umode_t mask)
{
	return may_create(dir, dentry, SECCLASS_DIR);
}