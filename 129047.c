asmlinkage long compat_sys_mount(char __user * dev_name, char __user * dir_name,
				 char __user * type, unsigned long flags,
				 void __user * data)
{
	unsigned long type_page;
	unsigned long data_page;
	unsigned long dev_page;
	char *dir_page;
	int retval;

	retval = copy_mount_options (type, &type_page);
	if (retval < 0)
		goto out;

	dir_page = getname(dir_name);
	retval = PTR_ERR(dir_page);
	if (IS_ERR(dir_page))
		goto out1;

	retval = copy_mount_options (dev_name, &dev_page);
	if (retval < 0)
		goto out2;

	retval = copy_mount_options (data, &data_page);
	if (retval < 0)
		goto out3;

	retval = -EINVAL;

	if (type_page && data_page) {
		if (!strcmp((char *)type_page, SMBFS_NAME)) {
			do_smb_super_data_conv((void *)data_page);
		} else if (!strcmp((char *)type_page, NCPFS_NAME)) {
			do_ncp_super_data_conv((void *)data_page);
		} else if (!strcmp((char *)type_page, NFS4_NAME)) {
			if (do_nfs4_super_data_conv((void *) data_page))
				goto out4;
		}
	}

	lock_kernel();
	retval = do_mount((char*)dev_page, dir_page, (char*)type_page,
			flags, (void*)data_page);
	unlock_kernel();

 out4:
	free_page(data_page);
 out3:
	free_page(dev_page);
 out2:
	putname(dir_page);
 out1:
	free_page(type_page);
 out:
	return retval;
}