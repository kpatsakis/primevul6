xfs_ioctl_setattr_check_projid(
	struct xfs_inode	*ip,
	struct fileattr		*fa)
{
	if (!fa->fsx_valid)
		return 0;

	/* Disallow 32bit project ids if 32bit IDs are not enabled. */
	if (fa->fsx_projid > (uint16_t)-1 &&
	    !xfs_has_projid32(ip->i_mount))
		return -EINVAL;
	return 0;
}