asmlinkage long sys_uselib(const char __user * library)
{
	struct file * file;
	struct nameidata nd;
	int error;

	error = __user_path_lookup_open(library, LOOKUP_FOLLOW, &nd, FMODE_READ|FMODE_EXEC);
	if (error)
		goto out;

	error = -EACCES;
	if (nd.mnt->mnt_flags & MNT_NOEXEC)
		goto exit;
	error = -EINVAL;
	if (!S_ISREG(nd.dentry->d_inode->i_mode))
		goto exit;

	error = vfs_permission(&nd, MAY_READ | MAY_EXEC);
	if (error)
		goto exit;

	file = nameidata_to_filp(&nd, O_RDONLY);
	error = PTR_ERR(file);
	if (IS_ERR(file))
		goto out;

	error = -ENOEXEC;
	if(file->f_op) {
		struct linux_binfmt * fmt;

		read_lock(&binfmt_lock);
		for (fmt = formats ; fmt ; fmt = fmt->next) {
			if (!fmt->load_shlib)
				continue;
			if (!try_module_get(fmt->module))
				continue;
			read_unlock(&binfmt_lock);
			error = fmt->load_shlib(file);
			read_lock(&binfmt_lock);
			put_binfmt(fmt);
			if (error != -ENOEXEC)
				break;
		}
		read_unlock(&binfmt_lock);
	}
	fput(file);
out:
  	return error;
exit:
	release_open_intent(&nd);
	path_release(&nd);
	goto out;
}