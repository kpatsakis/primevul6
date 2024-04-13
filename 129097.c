static int compat_nfs_getfd_trans(struct nfsctl_arg *karg,
				struct compat_nfsctl_arg __user *arg)
{
	if (!access_ok(VERIFY_READ, &arg->ca32_getfd,
			sizeof(arg->ca32_getfd)) ||
		get_user(karg->ca_version, &arg->ca32_version) ||
		__copy_from_user(&karg->ca_getfd.gd_addr,
				&arg->ca32_getfd.gd32_addr,
				(sizeof(struct sockaddr))) ||
		__copy_from_user(&karg->ca_getfd.gd_path,
				&arg->ca32_getfd.gd32_path,
				(NFS_MAXPATHLEN+1)) ||
		__get_user(karg->ca_getfd.gd_version,
				&arg->ca32_getfd.gd32_version))
		return -EFAULT;

	return 0;
}