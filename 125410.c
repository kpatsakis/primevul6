xfs_ioc_attr_list(
	struct xfs_inode		*dp,
	void __user			*ubuf,
	int				bufsize,
	int				flags,
	struct xfs_attrlist_cursor __user *ucursor)
{
	struct xfs_attr_list_context	context = { };
	struct xfs_attrlist		*alist;
	void				*buffer;
	int				error;

	if (bufsize < sizeof(struct xfs_attrlist) ||
	    bufsize > XFS_XATTR_LIST_MAX)
		return -EINVAL;

	/*
	 * Reject flags, only allow namespaces.
	 */
	if (flags & ~(XFS_IOC_ATTR_ROOT | XFS_IOC_ATTR_SECURE))
		return -EINVAL;
	if (flags == (XFS_IOC_ATTR_ROOT | XFS_IOC_ATTR_SECURE))
		return -EINVAL;

	/*
	 * Validate the cursor.
	 */
	if (copy_from_user(&context.cursor, ucursor, sizeof(context.cursor)))
		return -EFAULT;
	if (context.cursor.pad1 || context.cursor.pad2)
		return -EINVAL;
	if (!context.cursor.initted &&
	    (context.cursor.hashval || context.cursor.blkno ||
	     context.cursor.offset))
		return -EINVAL;

	buffer = kvzalloc(bufsize, GFP_KERNEL);
	if (!buffer)
		return -ENOMEM;

	/*
	 * Initialize the output buffer.
	 */
	context.dp = dp;
	context.resynch = 1;
	context.attr_filter = xfs_attr_filter(flags);
	context.buffer = buffer;
	context.bufsize = round_down(bufsize, sizeof(uint32_t));
	context.firstu = context.bufsize;
	context.put_listent = xfs_ioc_attr_put_listent;

	alist = context.buffer;
	alist->al_count = 0;
	alist->al_more = 0;
	alist->al_offset[0] = context.bufsize;

	error = xfs_attr_list(&context);
	if (error)
		goto out_free;

	if (copy_to_user(ubuf, buffer, bufsize) ||
	    copy_to_user(ucursor, &context.cursor, sizeof(context.cursor)))
		error = -EFAULT;
out_free:
	kmem_free(buffer);
	return error;
}