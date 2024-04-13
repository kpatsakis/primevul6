xfs_ioc_inumbers(
	struct xfs_mount		*mp,
	unsigned int			cmd,
	struct xfs_inumbers_req __user	*arg)
{
	struct xfs_bulk_ireq		hdr;
	struct xfs_ibulk		breq = {
		.mp			= mp,
	};
	int				error;

	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;

	if (xfs_is_shutdown(mp))
		return -EIO;

	if (copy_from_user(&hdr, &arg->hdr, sizeof(hdr)))
		return -EFAULT;

	error = xfs_bulk_ireq_setup(mp, &hdr, &breq, arg->inumbers);
	if (error == -ECANCELED)
		goto out_teardown;
	if (error < 0)
		return error;

	error = xfs_inumbers(&breq, xfs_inumbers_fmt);
	if (error)
		return error;

out_teardown:
	xfs_bulk_ireq_teardown(&hdr, &breq);
	if (copy_to_user(&arg->hdr, &hdr, sizeof(hdr)))
		return -EFAULT;

	return 0;
}