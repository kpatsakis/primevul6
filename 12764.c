static int smack_inode_notifysecctx(struct inode *inode, void *ctx, u32 ctxlen)
{
	return smack_inode_setsecurity(inode, XATTR_SMACK_SUFFIX, ctx,
				       ctxlen, 0);
}