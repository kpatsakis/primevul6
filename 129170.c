static int compat_count(compat_uptr_t __user *argv, int max)
{
	int i = 0;

	if (argv != NULL) {
		for (;;) {
			compat_uptr_t p;

			if (get_user(p, argv))
				return -EFAULT;
			if (!p)
				break;
			argv++;
			if(++i > max)
				return -E2BIG;
		}
	}
	return i;
}