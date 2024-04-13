static int compat_nfs_getfh_res_trans(union nfsctl_res *kres,
				union compat_nfsctl_res __user *res)
{
	int err;

	err = copy_to_user(res, kres, sizeof(*res));

	return (err) ? -EFAULT : 0;
}