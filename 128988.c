asmlinkage long compat_sys_newfstat(unsigned int fd,
		struct compat_stat __user * statbuf)
{
	struct kstat stat;
	int error = vfs_fstat(fd, &stat);

	if (!error)
		error = cp_compat_stat(&stat, statbuf);
	return error;
}