asmlinkage long compat_sys_epoll_wait(int epfd,
			struct compat_epoll_event __user *events,
			int maxevents, int timeout)
{
	long i, ret, err = 0;
	struct epoll_event __user *kbuf;
	struct epoll_event ev;

	if ((maxevents <= 0) ||
			(maxevents > (INT_MAX / sizeof(struct epoll_event))))
		return -EINVAL;
	kbuf = compat_alloc_user_space(sizeof(struct epoll_event) * maxevents);
	ret = sys_epoll_wait(epfd, kbuf, maxevents, timeout);
	for (i = 0; i < ret; i++) {
		err |= __get_user(ev.events, &kbuf[i].events);
		err |= __get_user(ev.data, &kbuf[i].data);
		err |= __put_user(ev.events, &events->events);
		err |= __put_user_unaligned(ev.data, &events->data);
		events++;
	}

	return err ? -EFAULT: ret;
}