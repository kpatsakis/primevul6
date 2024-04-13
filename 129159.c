asmlinkage long compat_sys_newfstatat(unsigned int dfd, char __user *filename,
		struct compat_stat __user *statbuf, int flag)
{
	struct kstat stat;
	int error = -EINVAL;

	if ((flag & ~AT_SYMLINK_NOFOLLOW) != 0)
		goto out;

	if (flag & AT_SYMLINK_NOFOLLOW)
		error = vfs_lstat_fd(dfd, filename, &stat);
	else
		error = vfs_stat_fd(dfd, filename, &stat);

	if (!error)
		error = cp_compat_stat(&stat, statbuf);

out:
	return error;
}