isdn_ppp_poll(struct file *file, poll_table *wait)
{
	u_int mask;
	struct ippp_buf_queue *bf, *bl;
	u_long flags;
	struct ippp_struct *is;

	is = file->private_data;

	if (is->debug & 0x2)
		printk(KERN_DEBUG "isdn_ppp_poll: minor: %d\n",
		       iminor(file_inode(file)));

	/* just registers wait_queue hook. This doesn't really wait. */
	poll_wait(file, &is->wq, wait);

	if (!(is->state & IPPP_OPEN)) {
		if (is->state == IPPP_CLOSEWAIT)
			return POLLHUP;
		printk(KERN_DEBUG "isdn_ppp: device not open\n");
		return POLLERR;
	}
	/* we're always ready to send .. */
	mask = POLLOUT | POLLWRNORM;

	spin_lock_irqsave(&is->buflock, flags);
	bl = is->last;
	bf = is->first;
	/*
	 * if IPPP_NOBLOCK is set we return even if we have nothing to read
	 */
	if (bf->next != bl || (is->state & IPPP_NOBLOCK)) {
		is->state &= ~IPPP_NOBLOCK;
		mask |= POLLIN | POLLRDNORM;
	}
	spin_unlock_irqrestore(&is->buflock, flags);
	return mask;
}