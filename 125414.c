xfs_attrmulti_attr_get(
	struct inode		*inode,
	unsigned char		*name,
	unsigned char		__user *ubuf,
	uint32_t		*len,
	uint32_t		flags)
{
	struct xfs_da_args	args = {
		.dp		= XFS_I(inode),
		.attr_filter	= xfs_attr_filter(flags),
		.attr_flags	= xfs_attr_flags(flags),
		.name		= name,
		.namelen	= strlen(name),
		.valuelen	= *len,
	};
	int			error;

	if (*len > XFS_XATTR_SIZE_MAX)
		return -EINVAL;

	error = xfs_attr_get(&args);
	if (error)
		goto out_kfree;

	*len = args.valuelen;
	if (copy_to_user(ubuf, args.value, args.valuelen))
		error = -EFAULT;

out_kfree:
	kmem_free(args.value);
	return error;
}