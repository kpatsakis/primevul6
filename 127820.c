isdn_ppp_write(int min, struct file *file, const char __user *buf, int count)
{
	isdn_net_local *lp;
	struct ippp_struct *is;
	int proto;
	unsigned char protobuf[4];

	is = file->private_data;

	if (!(is->state & IPPP_CONNECT))
		return 0;

	lp = is->lp;

	/* -> push it directly to the lowlevel interface */

	if (!lp)
		printk(KERN_DEBUG "isdn_ppp_write: lp == NULL\n");
	else {
		/*
		 * Don't reset huptimer for
		 * LCP packets. (Echo requests).
		 */
		if (copy_from_user(protobuf, buf, 4))
			return -EFAULT;
		proto = PPP_PROTOCOL(protobuf);
		if (proto != PPP_LCP)
			lp->huptimer = 0;

		if (lp->isdn_device < 0 || lp->isdn_channel < 0)
			return 0;

		if ((dev->drv[lp->isdn_device]->flags & DRV_FLAG_RUNNING) &&
		    lp->dialstate == 0 &&
		    (lp->flags & ISDN_NET_CONNECTED)) {
			unsigned short hl;
			struct sk_buff *skb;
			/*
			 * we need to reserve enough space in front of
			 * sk_buff. old call to dev_alloc_skb only reserved
			 * 16 bytes, now we are looking what the driver want
			 */
			hl = dev->drv[lp->isdn_device]->interface->hl_hdrlen;
			skb = alloc_skb(hl + count, GFP_ATOMIC);
			if (!skb) {
				printk(KERN_WARNING "isdn_ppp_write: out of memory!\n");
				return count;
			}
			skb_reserve(skb, hl);
			if (copy_from_user(skb_put(skb, count), buf, count))
			{
				kfree_skb(skb);
				return -EFAULT;
			}
			if (is->debug & 0x40) {
				printk(KERN_DEBUG "ppp xmit: len %d\n", (int) skb->len);
				isdn_ppp_frame_log("xmit", skb->data, skb->len, 32, is->unit, lp->ppp_slot);
			}

			isdn_ppp_send_ccp(lp->netdev, lp, skb); /* keeps CCP/compression states in sync */

			isdn_net_write_super(lp, skb);
		}
	}
	return count;
}