xfs_ioc_scrub_metadata(
	struct file			*file,
	void				__user *arg)
{
	struct xfs_scrub_metadata	scrub;
	int				error;

	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;

	if (copy_from_user(&scrub, arg, sizeof(scrub)))
		return -EFAULT;

	error = xfs_scrub_metadata(file, &scrub);
	if (error)
		return error;

	if (copy_to_user(arg, &scrub, sizeof(scrub)))
		return -EFAULT;

	return 0;
}