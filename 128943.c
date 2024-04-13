asmlinkage long compat_sys_signalfd(int ufd,
				    const compat_sigset_t __user *sigmask,
				    compat_size_t sigsetsize)
{
	compat_sigset_t ss32;
	sigset_t tmp;
	sigset_t __user *ksigmask;

	if (sigsetsize != sizeof(compat_sigset_t))
		return -EINVAL;
	if (copy_from_user(&ss32, sigmask, sizeof(ss32)))
		return -EFAULT;
	sigset_from_compat(&tmp, &ss32);
	ksigmask = compat_alloc_user_space(sizeof(sigset_t));
	if (copy_to_user(ksigmask, &tmp, sizeof(sigset_t)))
		return -EFAULT;

	return sys_signalfd(ufd, ksigmask, sizeof(sigset_t));
}