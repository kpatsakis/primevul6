xfs_ioc_ag_geometry(
	struct xfs_mount	*mp,
	void			__user *arg)
{
	struct xfs_ag_geometry	ageo;
	int			error;

	if (copy_from_user(&ageo, arg, sizeof(ageo)))
		return -EFAULT;
	if (ageo.ag_flags)
		return -EINVAL;
	if (memchr_inv(&ageo.ag_reserved, 0, sizeof(ageo.ag_reserved)))
		return -EINVAL;

	error = xfs_ag_get_geometry(mp, ageo.ag_number, &ageo);
	if (error)
		return error;

	if (copy_to_user(arg, &ageo, sizeof(ageo)))
		return -EFAULT;
	return 0;
}