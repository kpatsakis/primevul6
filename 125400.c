xfs_ioctl_setattr_check_cowextsize(
	struct xfs_inode	*ip,
	struct fileattr		*fa)
{
	struct xfs_mount	*mp = ip->i_mount;
	xfs_failaddr_t		failaddr;
	uint64_t		new_diflags2;
	uint16_t		new_diflags;

	if (!fa->fsx_valid)
		return 0;

	if (fa->fsx_cowextsize & mp->m_blockmask)
		return -EINVAL;

	new_diflags = xfs_flags2diflags(ip, fa->fsx_xflags);
	new_diflags2 = xfs_flags2diflags2(ip, fa->fsx_xflags);

	failaddr = xfs_inode_validate_cowextsize(ip->i_mount,
			XFS_B_TO_FSB(mp, fa->fsx_cowextsize),
			VFS_I(ip)->i_mode, new_diflags, new_diflags2);
	return failaddr != NULL ? -EINVAL : 0;
}