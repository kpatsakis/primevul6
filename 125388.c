xfs_ioctl_setattr_get_trans(
	struct xfs_inode	*ip,
	struct xfs_dquot	*pdqp)
{
	struct xfs_mount	*mp = ip->i_mount;
	struct xfs_trans	*tp;
	int			error = -EROFS;

	if (xfs_is_readonly(mp))
		goto out_error;
	error = -EIO;
	if (xfs_is_shutdown(mp))
		goto out_error;

	error = xfs_trans_alloc_ichange(ip, NULL, NULL, pdqp,
			capable(CAP_FOWNER), &tp);
	if (error)
		goto out_error;

	if (xfs_has_wsync(mp))
		xfs_trans_set_sync(tp);

	return tp;

out_error:
	return ERR_PTR(error);
}