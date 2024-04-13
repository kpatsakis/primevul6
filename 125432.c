xfs_attr_filter(
	u32			ioc_flags)
{
	if (ioc_flags & XFS_IOC_ATTR_ROOT)
		return XFS_ATTR_ROOT;
	if (ioc_flags & XFS_IOC_ATTR_SECURE)
		return XFS_ATTR_SECURE;
	return 0;
}