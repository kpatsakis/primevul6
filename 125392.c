xfs_ioc_fsgetxattra(
	xfs_inode_t		*ip,
	void			__user *arg)
{
	struct fileattr		fa;

	xfs_ilock(ip, XFS_ILOCK_SHARED);
	xfs_fill_fsxattr(ip, XFS_ATTR_FORK, &fa);
	xfs_iunlock(ip, XFS_ILOCK_SHARED);

	return copy_fsxattr_to_user(&fa, arg);
}