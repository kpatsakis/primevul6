static int compat_nfs_getfs_trans(struct nfsctl_arg *karg,
				struct compat_nfsctl_arg __user *arg)
{
	if (!access_ok(VERIFY_READ,&arg->ca32_getfs,sizeof(arg->ca32_getfs)) ||
		get_user(karg->ca_version, &arg->ca32_version) ||
		__copy_from_user(&karg->ca_getfs.gd_addr,
				&arg->ca32_getfs.gd32_addr,
				(sizeof(struct sockaddr))) ||
		__copy_from_user(&karg->ca_getfs.gd_path,
				&arg->ca32_getfs.gd32_path,
				(NFS_MAXPATHLEN+1)) ||
		__get_user(karg->ca_getfs.gd_maxlen,
				&arg->ca32_getfs.gd32_maxlen))
		return -EFAULT;

	return 0;
}