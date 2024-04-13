xfs_ioctl_setattr_prepare_dax(
	struct xfs_inode	*ip,
	struct fileattr		*fa)
{
	struct xfs_mount	*mp = ip->i_mount;
	struct inode            *inode = VFS_I(ip);

	if (S_ISDIR(inode->i_mode))
		return;

	if (xfs_has_dax_always(mp) || xfs_has_dax_never(mp))
		return;

	if (((fa->fsx_xflags & FS_XFLAG_DAX) &&
	    !(ip->i_diflags2 & XFS_DIFLAG2_DAX)) ||
	    (!(fa->fsx_xflags & FS_XFLAG_DAX) &&
	     (ip->i_diflags2 & XFS_DIFLAG2_DAX)))
		d_mark_dontcache(inode);
}