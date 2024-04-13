asmlinkage long compat_sys_utime(char __user *filename, struct compat_utimbuf __user *t)
{
	struct timespec tv[2];

	if (t) {
		if (get_user(tv[0].tv_sec, &t->actime) ||
		    get_user(tv[1].tv_sec, &t->modtime))
			return -EFAULT;
		tv[0].tv_nsec = 0;
		tv[1].tv_nsec = 0;
	}
	return do_utimes(AT_FDCWD, filename, t ? tv : NULL, 0);
}