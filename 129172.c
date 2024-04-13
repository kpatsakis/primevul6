asmlinkage long compat_sys_newstat(char __user * filename,
		struct compat_stat __user *statbuf)
{
	struct kstat stat;
	int error = vfs_stat_fd(AT_FDCWD, filename, &stat);

	if (!error)
		error = cp_compat_stat(&stat, statbuf);
	return error;
}