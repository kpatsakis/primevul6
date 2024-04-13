int unregister_binfmt(struct linux_binfmt * fmt)
{
	struct linux_binfmt ** tmp = &formats;

	write_lock(&binfmt_lock);
	while (*tmp) {
		if (fmt == *tmp) {
			*tmp = fmt->next;
			fmt->next = NULL;
			write_unlock(&binfmt_lock);
			return 0;
		}
		tmp = &(*tmp)->next;
	}
	write_unlock(&binfmt_lock);
	return -EINVAL;
}