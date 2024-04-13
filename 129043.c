int compat_set_fd_set(unsigned long nr, compat_ulong_t __user *ufdset,
		      unsigned long *fdset)
{
	unsigned long odd;
	nr = DIV_ROUND_UP(nr, __COMPAT_NFDBITS);

	if (!ufdset)
		return 0;

	odd = nr & 1UL;
	nr &= ~1UL;
	while (nr) {
		unsigned long h, l;
		l = *fdset++;
		h = l >> 32;
		if (__put_user(l, ufdset) || __put_user(h, ufdset+1))
			return -EFAULT;
		ufdset += 2;
		nr -= 2;
	}
	if (odd && __put_user(*fdset, ufdset))
		return -EFAULT;
	return 0;
}