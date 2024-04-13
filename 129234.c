static int compat_nfs_clnt_trans(struct nfsctl_arg *karg,
				struct compat_nfsctl_arg __user *arg)
{
	if (!access_ok(VERIFY_READ, &arg->ca32_client,
			sizeof(arg->ca32_client)) ||
		get_user(karg->ca_version, &arg->ca32_version) ||
		__copy_from_user(&karg->ca_client.cl_ident[0],
				&arg->ca32_client.cl32_ident[0],
				NFSCLNT_IDMAX) ||
		__get_user(karg->ca_client.cl_naddr,
				&arg->ca32_client.cl32_naddr) ||
		__copy_from_user(&karg->ca_client.cl_addrlist[0],
				&arg->ca32_client.cl32_addrlist[0],
				(sizeof(struct in_addr) * NFSCLNT_ADDRMAX)) ||
		__get_user(karg->ca_client.cl_fhkeytype,
				&arg->ca32_client.cl32_fhkeytype) ||
		__get_user(karg->ca_client.cl_fhkeylen,
				&arg->ca32_client.cl32_fhkeylen) ||
		__copy_from_user(&karg->ca_client.cl_fhkey[0],
				&arg->ca32_client.cl32_fhkey[0],
				NFSCLNT_KEYMAX))
		return -EFAULT;

	return 0;
}