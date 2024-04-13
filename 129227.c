asmlinkage long compat_sys_epoll_pwait(int epfd,
			struct compat_epoll_event __user *events,
			int maxevents, int timeout,
			const compat_sigset_t __user *sigmask,
			compat_size_t sigsetsize)
{
	long err;
	compat_sigset_t csigmask;
	sigset_t ksigmask, sigsaved;

	/*
	 * If the caller wants a certain signal mask to be set during the wait,
	 * we apply it here.
	 */
	if (sigmask) {
		if (sigsetsize != sizeof(compat_sigset_t))
			return -EINVAL;
		if (copy_from_user(&csigmask, sigmask, sizeof(csigmask)))
			return -EFAULT;
		sigset_from_compat(&ksigmask, &csigmask);
		sigdelsetmask(&ksigmask, sigmask(SIGKILL) | sigmask(SIGSTOP));
		sigprocmask(SIG_SETMASK, &ksigmask, &sigsaved);
	}

#ifdef CONFIG_HAS_COMPAT_EPOLL_EVENT
	err = compat_sys_epoll_wait(epfd, events, maxevents, timeout);
#else
	err = sys_epoll_wait(epfd, events, maxevents, timeout);
#endif

	/*
	 * If we changed the signal mask, we need to restore the original one.
	 * In case we've got a signal while waiting, we do not restore the
	 * signal mask yet, and we allow do_signal() to deliver the signal on
	 * the way back to userspace, before the signal mask is restored.
	 */
	if (sigmask) {
		if (err == -EINTR) {
			memcpy(&current->saved_sigmask, &sigsaved,
			       sizeof(sigsaved));
			set_thread_flag(TIF_RESTORE_SIGMASK);
		} else
			sigprocmask(SIG_SETMASK, &sigsaved, NULL);
	}

	return err;
}