compat_sys_writev(unsigned long fd, const struct compat_iovec __user *vec, unsigned long vlen)
{
	struct file *file;
	ssize_t ret = -EBADF;

	file = fget(fd);
	if (!file)
		return -EBADF;
	if (!(file->f_mode & FMODE_WRITE))
		goto out;

	ret = -EINVAL;
	if (!file->f_op || (!file->f_op->aio_write && !file->f_op->write))
		goto out;

	ret = compat_do_readv_writev(WRITE, file, vec, vlen, &file->f_pos);

out:
	fput(file);
	return ret;
}