static int smack_inode_getsecctx(struct inode *inode, void **ctx, u32 *ctxlen)
{
	struct smack_known *skp = smk_of_inode(inode);

	*ctx = skp->smk_known;
	*ctxlen = strlen(skp->smk_known);
	return 0;
}