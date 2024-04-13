asmlinkage long compat_sys_statfs(const char __user *path, struct compat_statfs __user *buf)
{
	struct nameidata nd;
	int error;

	error = user_path_walk(path, &nd);
	if (!error) {
		struct kstatfs tmp;
		error = vfs_statfs(nd.dentry, &tmp);
		if (!error)
			error = put_compat_statfs(buf, &tmp);
		path_release(&nd);
	}
	return error;
}