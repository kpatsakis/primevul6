compat_sys_open(const char __user *filename, int flags, int mode)
{
	return do_sys_open(AT_FDCWD, filename, flags, mode);
}