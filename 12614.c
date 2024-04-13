static void init_inode_smack(struct inode *inode, struct smack_known *skp)
{
	struct inode_smack *isp = smack_inode(inode);

	isp->smk_inode = skp;
	isp->smk_flags = 0;
}