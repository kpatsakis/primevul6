xfs_ioctl_setattr_xflags(
	struct xfs_trans	*tp,
	struct xfs_inode	*ip,
	struct fileattr		*fa)
{
	struct xfs_mount	*mp = ip->i_mount;
	uint64_t		i_flags2;

	/* Can't change realtime flag if any extents are allocated. */
	if ((ip->i_df.if_nextents || ip->i_delayed_blks) &&
	    XFS_IS_REALTIME_INODE(ip) != (fa->fsx_xflags & FS_XFLAG_REALTIME))
		return -EINVAL;

	/* If realtime flag is set then must have realtime device */
	if (fa->fsx_xflags & FS_XFLAG_REALTIME) {
		if (mp->m_sb.sb_rblocks == 0 || mp->m_sb.sb_rextsize == 0 ||
		    (ip->i_extsize % mp->m_sb.sb_rextsize))
			return -EINVAL;
	}

	/* Clear reflink if we are actually able to set the rt flag. */
	if ((fa->fsx_xflags & FS_XFLAG_REALTIME) && xfs_is_reflink_inode(ip))
		ip->i_diflags2 &= ~XFS_DIFLAG2_REFLINK;

	/* Don't allow us to set DAX mode for a reflinked file for now. */
	if ((fa->fsx_xflags & FS_XFLAG_DAX) && xfs_is_reflink_inode(ip))
		return -EINVAL;

	/* diflags2 only valid for v3 inodes. */
	i_flags2 = xfs_flags2diflags2(ip, fa->fsx_xflags);
	if (i_flags2 && !xfs_has_v3inodes(mp))
		return -EINVAL;

	ip->i_diflags = xfs_flags2diflags(ip, fa->fsx_xflags);
	ip->i_diflags2 = i_flags2;

	xfs_diflags_to_iflags(ip, false);
	xfs_trans_ichgtime(tp, ip, XFS_ICHGTIME_CHG);
	xfs_trans_log_inode(tp, ip, XFS_ILOG_CORE);
	XFS_STATS_INC(mp, xs_ig_attrchg);
	return 0;
}