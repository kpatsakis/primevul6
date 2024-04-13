static void smack_inode_getsecid(struct inode *inode, u32 *secid)
{
	struct smack_known *skp = smk_of_inode(inode);

	*secid = skp->smk_secid;
}