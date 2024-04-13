static int selinux_inode_create(struct inode *dir, struct dentry *dentry, umode_t mode)
{
	return may_create(dir, dentry, SECCLASS_FILE);
}