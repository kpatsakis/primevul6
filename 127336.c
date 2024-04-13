static int set_new_tls(struct task_struct *p, unsigned long tls)
{
	struct user_desc __user *utls = (struct user_desc __user *)tls;

	if (in_ia32_syscall())
		return do_set_thread_area(p, -1, utls, 0);
	else
		return do_set_thread_area_64(p, ARCH_SET_FS, tls);
}