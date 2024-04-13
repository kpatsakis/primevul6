compat_sys_io_getevents(aio_context_t ctx_id,
				 unsigned long min_nr,
				 unsigned long nr,
				 struct io_event __user *events,
				 struct compat_timespec __user *timeout)
{
	long ret;
	struct timespec t;
	struct timespec __user *ut = NULL;

	ret = -EFAULT;
	if (unlikely(!access_ok(VERIFY_WRITE, events, 
				nr * sizeof(struct io_event))))
		goto out;
	if (timeout) {
		if (get_compat_timespec(&t, timeout))
			goto out;

		ut = compat_alloc_user_space(sizeof(*ut));
		if (copy_to_user(ut, &t, sizeof(t)) )
			goto out;
	} 
	ret = sys_io_getevents(ctx_id, min_nr, nr, events, ut);
out:
	return ret;
}