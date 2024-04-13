static int compat_nfs_svc_trans(struct nfsctl_arg *karg,
				struct compat_nfsctl_arg __user *arg)
{
	if (!access_ok(VERIFY_READ, &arg->ca32_svc, sizeof(arg->ca32_svc)) ||
		get_user(karg->ca_version, &arg->ca32_version) ||
		__get_user(karg->ca_svc.svc_port, &arg->ca32_svc.svc32_port) ||
		__get_user(karg->ca_svc.svc_nthreads,
				&arg->ca32_svc.svc32_nthreads))
		return -EFAULT;
	return 0;
}