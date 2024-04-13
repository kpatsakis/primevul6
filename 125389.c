xfs_ioc_getbmap(
	struct file		*file,
	unsigned int		cmd,
	void			__user *arg)
{
	struct getbmapx		bmx = { 0 };
	struct kgetbmap		*buf;
	size_t			recsize;
	int			error, i;

	switch (cmd) {
	case XFS_IOC_GETBMAPA:
		bmx.bmv_iflags = BMV_IF_ATTRFORK;
		fallthrough;
	case XFS_IOC_GETBMAP:
		/* struct getbmap is a strict subset of struct getbmapx. */
		recsize = sizeof(struct getbmap);
		break;
	case XFS_IOC_GETBMAPX:
		recsize = sizeof(struct getbmapx);
		break;
	default:
		return -EINVAL;
	}

	if (copy_from_user(&bmx, arg, recsize))
		return -EFAULT;

	if (bmx.bmv_count < 2)
		return -EINVAL;
	if (bmx.bmv_count > ULONG_MAX / recsize)
		return -ENOMEM;

	buf = kvcalloc(bmx.bmv_count, sizeof(*buf), GFP_KERNEL);
	if (!buf)
		return -ENOMEM;

	error = xfs_getbmap(XFS_I(file_inode(file)), &bmx, buf);
	if (error)
		goto out_free_buf;

	error = -EFAULT;
	if (copy_to_user(arg, &bmx, recsize))
		goto out_free_buf;
	arg += recsize;

	for (i = 0; i < bmx.bmv_entries; i++) {
		if (!xfs_getbmap_format(buf + i, arg, recsize))
			goto out_free_buf;
		arg += recsize;
	}

	error = 0;
out_free_buf:
	kmem_free(buf);
	return error;
}