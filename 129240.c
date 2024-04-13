asmlinkage long compat_sys_old_readdir(unsigned int fd,
	struct compat_old_linux_dirent __user *dirent, unsigned int count)
{
	int error;
	struct file *file;
	struct compat_readdir_callback buf;

	error = -EBADF;
	file = fget(fd);
	if (!file)
		goto out;

	buf.result = 0;
	buf.dirent = dirent;

	error = vfs_readdir(file, compat_fillonedir, &buf);
	if (error >= 0)
		error = buf.result;

	fput(file);
out:
	return error;
}