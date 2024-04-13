static int selinux_inode_mknod(struct inode *dir, struct dentry *dentry, umode_t mode, dev_t dev)
{
	return may_create(dir, dentry, inode_mode_to_security_class(mode));
}