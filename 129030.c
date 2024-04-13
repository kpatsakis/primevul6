asmlinkage long compat_sys_getdents(unsigned int fd,
		struct compat_linux_dirent __user *dirent, unsigned int count)
{
	struct file * file;
	struct compat_linux_dirent __user * lastdirent;
	struct compat_getdents_callback buf;
	int error;

	error = -EFAULT;
	if (!access_ok(VERIFY_WRITE, dirent, count))
		goto out;

	error = -EBADF;
	file = fget(fd);
	if (!file)
		goto out;

	buf.current_dir = dirent;
	buf.previous = NULL;
	buf.count = count;
	buf.error = 0;

	error = vfs_readdir(file, compat_filldir, &buf);
	if (error < 0)
		goto out_putf;
	error = buf.error;
	lastdirent = buf.previous;
	if (lastdirent) {
		if (put_user(file->f_pos, &lastdirent->d_off))
			error = -EFAULT;
		else
			error = count - buf.count;
	}

out_putf:
	fput(file);
out:
	return error;
}