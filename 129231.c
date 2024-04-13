static int compat_nfs_exp_trans(struct nfsctl_arg *karg,
				struct compat_nfsctl_arg __user *arg)
{
	if (!access_ok(VERIFY_READ, &arg->ca32_export,
				sizeof(arg->ca32_export)) ||
		get_user(karg->ca_version, &arg->ca32_version) ||
		__copy_from_user(&karg->ca_export.ex_client[0],
				&arg->ca32_export.ex32_client[0],
				NFSCLNT_IDMAX) ||
		__copy_from_user(&karg->ca_export.ex_path[0],
				&arg->ca32_export.ex32_path[0],
				NFS_MAXPATHLEN) ||
		__get_user(karg->ca_export.ex_dev,
				&arg->ca32_export.ex32_dev) ||
		__get_user(karg->ca_export.ex_ino,
				&arg->ca32_export.ex32_ino) ||
		__get_user(karg->ca_export.ex_flags,
				&arg->ca32_export.ex32_flags) ||
		__get_user(karg->ca_export.ex_anon_uid,
				&arg->ca32_export.ex32_anon_uid) ||
		__get_user(karg->ca_export.ex_anon_gid,
				&arg->ca32_export.ex32_anon_gid))
		return -EFAULT;
	SET_UID(karg->ca_export.ex_anon_uid, karg->ca_export.ex_anon_uid);
	SET_GID(karg->ca_export.ex_anon_gid, karg->ca_export.ex_anon_gid);

	return 0;
}