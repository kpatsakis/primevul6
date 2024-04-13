xfs_attr_flags(
	u32			ioc_flags)
{
	if (ioc_flags & XFS_IOC_ATTR_CREATE)
		return XATTR_CREATE;
	if (ioc_flags & XFS_IOC_ATTR_REPLACE)
		return XATTR_REPLACE;
	return 0;
}