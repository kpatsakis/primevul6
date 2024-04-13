isdn_ppp_read(int min, struct file *file, char __user *buf, int count)
{
	struct ippp_struct *is;
	struct ippp_buf_queue *b;
	u_long flags;
	u_char *save_buf;

	is = file->private_data;

	if (!(is->state & IPPP_OPEN))
		return 0;

	if (!access_ok(VERIFY_WRITE, buf, count))
		return -EFAULT;

	spin_lock_irqsave(&is->buflock, flags);
	b = is->first->next;
	save_buf = b->buf;
	if (!save_buf) {
		spin_unlock_irqrestore(&is->buflock, flags);
		return -EAGAIN;
	}
	if (b->len < count)
		count = b->len;
	b->buf = NULL;
	is->first = b;

	spin_unlock_irqrestore(&is->buflock, flags);
	if (copy_to_user(buf, save_buf, count))
		count = -EFAULT;
	kfree(save_buf);

	return count;
}