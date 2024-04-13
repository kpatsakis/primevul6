xfs_readlink_by_handle(
	struct file		*parfilp,
	xfs_fsop_handlereq_t	*hreq)
{
	struct dentry		*dentry;
	__u32			olen;
	int			error;

	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;

	dentry = xfs_handlereq_to_dentry(parfilp, hreq);
	if (IS_ERR(dentry))
		return PTR_ERR(dentry);

	/* Restrict this handle operation to symlinks only. */
	if (!d_is_symlink(dentry)) {
		error = -EINVAL;
		goto out_dput;
	}

	if (copy_from_user(&olen, hreq->ohandlen, sizeof(__u32))) {
		error = -EFAULT;
		goto out_dput;
	}

	error = vfs_readlink(dentry, hreq->ohandle, olen);

 out_dput:
	dput(dentry);
	return error;
}