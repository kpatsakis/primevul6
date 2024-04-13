xfs_fill_fsxattr(
	struct xfs_inode	*ip,
	int			whichfork,
	struct fileattr		*fa)
{
	struct xfs_mount	*mp = ip->i_mount;
	struct xfs_ifork	*ifp = XFS_IFORK_PTR(ip, whichfork);

	fileattr_fill_xflags(fa, xfs_ip2xflags(ip));

	if (ip->i_diflags & XFS_DIFLAG_EXTSIZE) {
		fa->fsx_extsize = XFS_FSB_TO_B(mp, ip->i_extsize);
	} else if (ip->i_diflags & XFS_DIFLAG_EXTSZINHERIT) {
		/*
		 * Don't let a misaligned extent size hint on a directory
		 * escape to userspace if it won't pass the setattr checks
		 * later.
		 */
		if ((ip->i_diflags & XFS_DIFLAG_RTINHERIT) &&
		    ip->i_extsize % mp->m_sb.sb_rextsize > 0) {
			fa->fsx_xflags &= ~(FS_XFLAG_EXTSIZE |
					    FS_XFLAG_EXTSZINHERIT);
			fa->fsx_extsize = 0;
		} else {
			fa->fsx_extsize = XFS_FSB_TO_B(mp, ip->i_extsize);
		}
	}

	if (ip->i_diflags2 & XFS_DIFLAG2_COWEXTSIZE)
		fa->fsx_cowextsize = XFS_FSB_TO_B(mp, ip->i_cowextsize);
	fa->fsx_projid = ip->i_projid;
	if (ifp && !xfs_need_iread_extents(ifp))
		fa->fsx_nextents = xfs_iext_count(ifp);
	else
		fa->fsx_nextents = xfs_ifork_nextents(ifp);
}