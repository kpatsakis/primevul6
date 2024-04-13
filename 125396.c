xfs_attrmulti_attr_set(
	struct inode		*inode,
	unsigned char		*name,
	const unsigned char	__user *ubuf,
	uint32_t		len,
	uint32_t		flags)
{
	struct xfs_da_args	args = {
		.dp		= XFS_I(inode),
		.attr_filter	= xfs_attr_filter(flags),
		.attr_flags	= xfs_attr_flags(flags),
		.name		= name,
		.namelen	= strlen(name),
	};
	int			error;

	if (IS_IMMUTABLE(inode) || IS_APPEND(inode))
		return -EPERM;

	if (ubuf) {
		if (len > XFS_XATTR_SIZE_MAX)
			return -EINVAL;
		args.value = memdup_user(ubuf, len);
		if (IS_ERR(args.value))
			return PTR_ERR(args.value);
		args.valuelen = len;
	}

	error = xfs_attr_set(&args);
	if (!error && (flags & XFS_IOC_ATTR_ROOT))
		xfs_forget_acl(inode, name);
	kfree(args.value);
	return error;
}