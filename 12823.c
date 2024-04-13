static int smack_inode_alloc_security(struct inode *inode)
{
	struct smack_known *skp = smk_of_current();

	init_inode_smack(inode, skp);
	return 0;
}