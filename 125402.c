xfs_attrmulti_by_handle(
	struct file		*parfilp,
	void			__user *arg)
{
	int			error;
	xfs_attr_multiop_t	*ops;
	xfs_fsop_attrmulti_handlereq_t am_hreq;
	struct dentry		*dentry;
	unsigned int		i, size;

	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;
	if (copy_from_user(&am_hreq, arg, sizeof(xfs_fsop_attrmulti_handlereq_t)))
		return -EFAULT;

	/* overflow check */
	if (am_hreq.opcount >= INT_MAX / sizeof(xfs_attr_multiop_t))
		return -E2BIG;

	dentry = xfs_handlereq_to_dentry(parfilp, &am_hreq.hreq);
	if (IS_ERR(dentry))
		return PTR_ERR(dentry);

	error = -E2BIG;
	size = am_hreq.opcount * sizeof(xfs_attr_multiop_t);
	if (!size || size > 16 * PAGE_SIZE)
		goto out_dput;

	ops = memdup_user(am_hreq.ops, size);
	if (IS_ERR(ops)) {
		error = PTR_ERR(ops);
		goto out_dput;
	}

	error = 0;
	for (i = 0; i < am_hreq.opcount; i++) {
		ops[i].am_error = xfs_ioc_attrmulti_one(parfilp,
				d_inode(dentry), ops[i].am_opcode,
				ops[i].am_attrname, ops[i].am_attrvalue,
				&ops[i].am_length, ops[i].am_flags);
	}

	if (copy_to_user(am_hreq.ops, ops, size))
		error = -EFAULT;

	kfree(ops);
 out_dput:
	dput(dentry);
	return error;
}