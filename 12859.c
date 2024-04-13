static int smack_inode_setsecctx(struct dentry *dentry, void *ctx, u32 ctxlen)
{
	return __vfs_setxattr_noperm(&init_user_ns, dentry, XATTR_NAME_SMACK,
				     ctx, ctxlen, 0);
}