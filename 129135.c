bm_entry_read(struct file * file, char __user * buf, size_t nbytes, loff_t *ppos)
{
	Node *e = file->f_path.dentry->d_inode->i_private;
	loff_t pos = *ppos;
	ssize_t res;
	char *page;
	int len;

	if (!(page = (char*) __get_free_page(GFP_KERNEL)))
		return -ENOMEM;

	entry_status(e, page);
	len = strlen(page);

	res = -EINVAL;
	if (pos < 0)
		goto out;
	res = 0;
	if (pos >= len)
		goto out;
	if (len < pos + nbytes)
		nbytes = len - pos;
	res = -EFAULT;
	if (copy_to_user(buf, page + pos, nbytes))
		goto out;
	*ppos = pos + nbytes;
	res = nbytes;
out:
	free_page((unsigned long) page);
	return res;
}