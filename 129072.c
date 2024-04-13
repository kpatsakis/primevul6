int register_binfmt(struct linux_binfmt * fmt)
{
	struct linux_binfmt ** tmp = &formats;

	if (!fmt)
		return -EINVAL;
	if (fmt->next)
		return -EBUSY;
	write_lock(&binfmt_lock);
	while (*tmp) {
		if (fmt == *tmp) {
			write_unlock(&binfmt_lock);
			return -EBUSY;
		}
		tmp = &(*tmp)->next;
	}
	fmt->next = formats;
	formats = fmt;
	write_unlock(&binfmt_lock);
	return 0;	
}