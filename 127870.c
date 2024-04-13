void isdn_ppp_receive(isdn_net_dev *net_dev, isdn_net_local *lp, struct sk_buff *skb)
{
	struct ippp_struct *is;
	int slot;
	int proto;

	BUG_ON(net_dev->local->master); // we're called with the master device always

	slot = lp->ppp_slot;
	if (slot < 0 || slot >= ISDN_MAX_CHANNELS) {
		printk(KERN_ERR "isdn_ppp_receive: lp->ppp_slot(%d)\n",
		       lp->ppp_slot);
		kfree_skb(skb);
		return;
	}
	is = ippp_table[slot];

	if (is->debug & 0x4) {
		printk(KERN_DEBUG "ippp_receive: is:%08lx lp:%08lx slot:%d unit:%d len:%d\n",
		       (long)is, (long)lp, lp->ppp_slot, is->unit, (int)skb->len);
		isdn_ppp_frame_log("receive", skb->data, skb->len, 32, is->unit, lp->ppp_slot);
	}

	if (isdn_ppp_skip_ac(is, skb) < 0) {
		kfree_skb(skb);
		return;
	}
	proto = isdn_ppp_strip_proto(skb);
	if (proto < 0) {
		kfree_skb(skb);
		return;
	}

#ifdef CONFIG_ISDN_MPP
	if (is->compflags & SC_LINK_DECOMP_ON) {
		skb = isdn_ppp_decompress(skb, is, NULL, &proto);
		if (!skb) // decompression error
			return;
	}

	if (!(is->mpppcfg & SC_REJ_MP_PROT)) { // we agreed to receive MPPP
		if (proto == PPP_MP) {
			isdn_ppp_mp_receive(net_dev, lp, skb);
			return;
		}
	}
#endif
	isdn_ppp_push_higher(net_dev, lp, skb, proto);
}