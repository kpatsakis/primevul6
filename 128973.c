static int compat_fillonedir(void *__buf, const char *name, int namlen,
			loff_t offset, u64 ino, unsigned int d_type)
{
	struct compat_readdir_callback *buf = __buf;
	struct compat_old_linux_dirent __user *dirent;
	compat_ulong_t d_ino;

	if (buf->result)
		return -EINVAL;
	d_ino = ino;
	if (sizeof(d_ino) < sizeof(ino) && d_ino != ino)
		return -EOVERFLOW;
	buf->result++;
	dirent = buf->dirent;
	if (!access_ok(VERIFY_WRITE, dirent,
			(unsigned long)(dirent->d_name + namlen + 1) -
				(unsigned long)dirent))
		goto efault;
	if (	__put_user(d_ino, &dirent->d_ino) ||
		__put_user(offset, &dirent->d_offset) ||
		__put_user(namlen, &dirent->d_namlen) ||
		__copy_to_user(dirent->d_name, name, namlen) ||
		__put_user(0, dirent->d_name + namlen))
		goto efault;
	return 0;
efault:
	buf->result = -EFAULT;
	return -EFAULT;
}