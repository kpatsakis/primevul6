asmlinkage long compat_sys_futimesat(unsigned int dfd, char __user *filename, struct compat_timeval __user *t)
{
	struct timespec tv[2];

	if (t) {
		if (get_user(tv[0].tv_sec, &t[0].tv_sec) ||
		    get_user(tv[0].tv_nsec, &t[0].tv_usec) ||
		    get_user(tv[1].tv_sec, &t[1].tv_sec) ||
		    get_user(tv[1].tv_nsec, &t[1].tv_usec))
			return -EFAULT;
		if (tv[0].tv_nsec >= 1000000 || tv[0].tv_nsec < 0 ||
		    tv[1].tv_nsec >= 1000000 || tv[1].tv_nsec < 0)
			return -EINVAL;
		tv[0].tv_nsec *= 1000;
		tv[1].tv_nsec *= 1000;
	}
	return do_utimes(dfd, filename, t ? tv : NULL, 0);
}