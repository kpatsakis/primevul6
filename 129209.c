asmlinkage long compat_sys_epoll_ctl(int epfd, int op, int fd,
			struct compat_epoll_event __user *event)
{
	long err = 0;
	struct compat_epoll_event user;
	struct epoll_event __user *kernel = NULL;

	if (event) {
		if (copy_from_user(&user, event, sizeof(user)))
			return -EFAULT;
		kernel = compat_alloc_user_space(sizeof(struct epoll_event));
		err |= __put_user(user.events, &kernel->events);
		err |= __put_user(user.data, &kernel->data);
	}

	return err ? err : sys_epoll_ctl(epfd, op, fd, kernel);
}