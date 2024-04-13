compat_sys_io_submit(aio_context_t ctx_id, int nr, u32 __user *iocb)
{
	struct iocb __user * __user *iocb64; 
	long ret;

	if (unlikely(nr < 0))
		return -EINVAL;

	if (nr > MAX_AIO_SUBMITS)
		nr = MAX_AIO_SUBMITS;
	
	iocb64 = compat_alloc_user_space(nr * sizeof(*iocb64));
	ret = copy_iocb(nr, iocb, iocb64);
	if (!ret)
		ret = sys_io_submit(ctx_id, nr, iocb64);
	return ret;
}