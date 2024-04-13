static void selinux_inode_free_security(struct inode *inode)
{
	inode_free_security(inode);
}