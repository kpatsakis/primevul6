isdn_ppp_closewait(int slot)
{
	struct ippp_struct *is;

	if (slot < 0 || slot >= ISDN_MAX_CHANNELS) {
		printk(KERN_ERR "%s: slot(%d) out of range\n",
		       __func__, slot);
		return 0;
	}
	is = ippp_table[slot];
	if (is->state)
		wake_up_interruptible(&is->wq);
	is->state = IPPP_CLOSEWAIT;
	return 1;
}