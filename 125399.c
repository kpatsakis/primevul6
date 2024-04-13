xfs_fileattr_get(
	struct dentry		*dentry,
	struct fileattr		*fa)
{
	struct xfs_inode	*ip = XFS_I(d_inode(dentry));

	if (d_is_special(dentry))
		return -ENOTTY;

	xfs_ilock(ip, XFS_ILOCK_SHARED);
	xfs_fill_fsxattr(ip, XFS_DATA_FORK, fa);
	xfs_iunlock(ip, XFS_ILOCK_SHARED);

	return 0;
}