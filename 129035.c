asmlinkage long compat_sys_utimes(char __user *filename, struct compat_timeval __user *t)
{
	return compat_sys_futimesat(AT_FDCWD, filename, t);
}