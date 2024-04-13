xfs_ioctl_setattr_check_extsize(
	struct xfs_inode	*ip,
	struct fileattr		*fa)
{
	struct xfs_mount	*mp = ip->i_mount;
	xfs_failaddr_t		failaddr;
	uint16_t		new_diflags;

	if (!fa->fsx_valid)
		return 0;

	if (S_ISREG(VFS_I(ip)->i_mode) && ip->i_df.if_nextents &&
	    XFS_FSB_TO_B(mp, ip->i_extsize) != fa->fsx_extsize)
		return -EINVAL;

	if (fa->fsx_extsize & mp->m_blockmask)
		return -EINVAL;

	new_diflags = xfs_flags2diflags(ip, fa->fsx_xflags);

	/*
	 * Inode verifiers do not check that the extent size hint is an integer
	 * multiple of the rt extent size on a directory with both rtinherit
	 * and extszinherit flags set.  Don't let sysadmins misconfigure
	 * directories.
	 */
	if ((new_diflags & XFS_DIFLAG_RTINHERIT) &&
	    (new_diflags & XFS_DIFLAG_EXTSZINHERIT)) {
		unsigned int	rtextsize_bytes;

		rtextsize_bytes = XFS_FSB_TO_B(mp, mp->m_sb.sb_rextsize);
		if (fa->fsx_extsize % rtextsize_bytes)
			return -EINVAL;
	}

	failaddr = xfs_inode_validate_extsize(ip->i_mount,
			XFS_B_TO_FSB(mp, fa->fsx_extsize),
			VFS_I(ip)->i_mode, new_diflags);
	return failaddr != NULL ? -EINVAL : 0;
}