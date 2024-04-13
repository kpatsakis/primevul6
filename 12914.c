static int selinux_inode_readlink(struct dentry *dentry)
{
	const struct cred *cred = current_cred();

	return dentry_has_perm(cred, dentry, FILE__READ);
}