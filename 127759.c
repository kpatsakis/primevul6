static void isdn_ppp_ccp_xmit_reset(struct ippp_struct *is, int proto,
				    unsigned char code, unsigned char id,
				    unsigned char *data, int len)
{
	struct sk_buff *skb;
	unsigned char *p;
	int hl;
	int cnt = 0;
	isdn_net_local *lp = is->lp;

	/* Alloc large enough skb */
	hl = dev->drv[lp->isdn_device]->interface->hl_hdrlen;
	skb = alloc_skb(len + hl + 16, GFP_ATOMIC);
	if (!skb) {
		printk(KERN_WARNING
		       "ippp: CCP cannot send reset - out of memory\n");
		return;
	}
	skb_reserve(skb, hl);

	/* We may need to stuff an address and control field first */
	if (!(is->pppcfg & SC_COMP_AC)) {
		p = skb_put(skb, 2);
		*p++ = 0xff;
		*p++ = 0x03;
	}

	/* Stuff proto, code, id and length */
	p = skb_put(skb, 6);
	*p++ = (proto >> 8);
	*p++ = (proto & 0xff);
	*p++ = code;
	*p++ = id;
	cnt = 4 + len;
	*p++ = (cnt >> 8);
	*p++ = (cnt & 0xff);

	/* Now stuff remaining bytes */
	if (len) {
		p = skb_put(skb, len);
		memcpy(p, data, len);
	}

	/* skb is now ready for xmit */
	printk(KERN_DEBUG "Sending CCP Frame:\n");
	isdn_ppp_frame_log("ccp-xmit", skb->data, skb->len, 32, is->unit, lp->ppp_slot);

	isdn_net_write_super(lp, skb);
}