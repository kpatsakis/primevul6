asmlinkage long compat_sys_utimensat(unsigned int dfd, char __user *filename, struct compat_timespec __user *t, int flags)
{
	struct timespec tv[2];

	if  (t) {
		if (get_compat_timespec(&tv[0], &t[0]) ||
		    get_compat_timespec(&tv[1], &t[1]))
			return -EFAULT;

		if ((tv[0].tv_nsec == UTIME_OMIT || tv[0].tv_nsec == UTIME_NOW)
		    && tv[0].tv_sec != 0)
			return -EINVAL;
		if ((tv[1].tv_nsec == UTIME_OMIT || tv[1].tv_nsec == UTIME_NOW)
		    && tv[1].tv_sec != 0)
			return -EINVAL;

		if (tv[0].tv_nsec == UTIME_OMIT && tv[1].tv_nsec == UTIME_OMIT)
			return 0;
	}
	return do_utimes(dfd, filename, t ? tv : NULL, flags);
}