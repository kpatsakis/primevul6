asmlinkage long compat_sys_timerfd(int ufd, int clockid, int flags,
				   const struct compat_itimerspec __user *utmr)
{
	struct itimerspec t;
	struct itimerspec __user *ut;

	if (get_compat_itimerspec(&t, utmr))
		return -EFAULT;
	ut = compat_alloc_user_space(sizeof(*ut));
	if (copy_to_user(ut, &t, sizeof(t)))
		return -EFAULT;

	return sys_timerfd(ufd, clockid, flags, ut);
}