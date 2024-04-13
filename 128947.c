copy_iocb(long nr, u32 __user *ptr32, struct iocb __user * __user *ptr64)
{
	compat_uptr_t uptr;
	int i;

	for (i = 0; i < nr; ++i) {
		if (get_user(uptr, ptr32 + i))
			return -EFAULT;
		if (put_user(compat_ptr(uptr), ptr64 + i))
			return -EFAULT;
	}
	return 0;
}