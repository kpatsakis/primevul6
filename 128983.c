asmlinkage long compat_sys_nfsservctl(int cmd,
				struct compat_nfsctl_arg __user *arg,
				union compat_nfsctl_res __user *res)
{
	struct nfsctl_arg *karg;
	union nfsctl_res *kres;
	mm_segment_t oldfs;
	int err;

	karg = kmalloc(sizeof(*karg), GFP_USER);
	kres = kmalloc(sizeof(*kres), GFP_USER);
	if(!karg || !kres) {
		err = -ENOMEM;
		goto done;
	}

	switch(cmd) {
	case NFSCTL_SVC:
		err = compat_nfs_svc_trans(karg, arg);
		break;

	case NFSCTL_ADDCLIENT:
		err = compat_nfs_clnt_trans(karg, arg);
		break;

	case NFSCTL_DELCLIENT:
		err = compat_nfs_clnt_trans(karg, arg);
		break;

	case NFSCTL_EXPORT:
	case NFSCTL_UNEXPORT:
		err = compat_nfs_exp_trans(karg, arg);
		break;

	case NFSCTL_GETFD:
		err = compat_nfs_getfd_trans(karg, arg);
		break;

	case NFSCTL_GETFS:
		err = compat_nfs_getfs_trans(karg, arg);
		break;

	default:
		err = -EINVAL;
		break;
	}

	if (err)
		goto done;

	oldfs = get_fs();
	set_fs(KERNEL_DS);
	/* The __user pointer casts are valid because of the set_fs() */
	err = sys_nfsservctl(cmd, (void __user *) karg, (void __user *) kres);
	set_fs(oldfs);

	if (err)
		goto done;

	if((cmd == NFSCTL_GETFD) ||
	   (cmd == NFSCTL_GETFS))
		err = compat_nfs_getfh_res_trans(kres, res);

done:
	kfree(karg);
	kfree(kres);
	return err;
}