xfs_ioc_bulkstat(
	struct file			*file,
	unsigned int			cmd,
	struct xfs_bulkstat_req __user	*arg)
{
	struct xfs_mount		*mp = XFS_I(file_inode(file))->i_mount;
	struct xfs_bulk_ireq		hdr;
	struct xfs_ibulk		breq = {
		.mp			= mp,
		.mnt_userns		= file_mnt_user_ns(file),
	};
	int				error;

	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;

	if (xfs_is_shutdown(mp))
		return -EIO;

	if (copy_from_user(&hdr, &arg->hdr, sizeof(hdr)))
		return -EFAULT;

	error = xfs_bulk_ireq_setup(mp, &hdr, &breq, arg->bulkstat);
	if (error == -ECANCELED)
		goto out_teardown;
	if (error < 0)
		return error;

	error = xfs_bulkstat(&breq, xfs_bulkstat_fmt);
	if (error)
		return error;

out_teardown:
	xfs_bulk_ireq_teardown(&hdr, &breq);
	if (copy_to_user(&arg->hdr, &hdr, sizeof(hdr)))
		return -EFAULT;

	return 0;
}