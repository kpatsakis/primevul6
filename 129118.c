asmlinkage long compat_sys_statfs64(const char __user *path, compat_size_t sz, struct compat_statfs64 __user *buf)
{
	struct nameidata nd;
	int error;

	if (sz != sizeof(*buf))
		return -EINVAL;

	error = user_path_walk(path, &nd);
	if (!error) {
		struct kstatfs tmp;
		error = vfs_statfs(nd.dentry, &tmp);
		if (!error)
			error = put_compat_statfs64(buf, &tmp);
		path_release(&nd);
	}
	return error;
}