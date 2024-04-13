isdn_ppp_fill_rq(unsigned char *buf, int len, int proto, int slot)
{
	struct ippp_buf_queue *bf, *bl;
	u_long flags;
	u_char *nbuf;
	struct ippp_struct *is;

	if (slot < 0 || slot >= ISDN_MAX_CHANNELS) {
		printk(KERN_WARNING "ippp: illegal slot(%d).\n", slot);
		return 0;
	}
	is = ippp_table[slot];

	if (!(is->state & IPPP_CONNECT)) {
		printk(KERN_DEBUG "ippp: device not activated.\n");
		return 0;
	}
	nbuf = kmalloc(len + 4, GFP_ATOMIC);
	if (!nbuf) {
		printk(KERN_WARNING "ippp: Can't alloc buf\n");
		return 0;
	}
	nbuf[0] = PPP_ALLSTATIONS;
	nbuf[1] = PPP_UI;
	nbuf[2] = proto >> 8;
	nbuf[3] = proto & 0xff;
	memcpy(nbuf + 4, buf, len);

	spin_lock_irqsave(&is->buflock, flags);
	bf = is->first;
	bl = is->last;

	if (bf == bl) {
		printk(KERN_WARNING "ippp: Queue is full; discarding first buffer\n");
		bf = bf->next;
		kfree(bf->buf);
		is->first = bf;
	}
	bl->buf = (char *) nbuf;
	bl->len = len + 4;

	is->last = bl->next;
	spin_unlock_irqrestore(&is->buflock, flags);
	wake_up_interruptible(&is->wq);
	return len;
}