compat_sys_openat(unsigned int dfd, const char __user *filename, int flags, int mode)
{
	return do_sys_open(dfd, filename, flags, mode);
}