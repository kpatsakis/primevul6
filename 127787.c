isdn_ppp_push_higher(isdn_net_dev *net_dev, isdn_net_local *lp, struct sk_buff *skb, int proto)
{
	struct net_device *dev = net_dev->dev;
	struct ippp_struct *is, *mis;
	isdn_net_local *mlp = NULL;
	int slot;

	slot = lp->ppp_slot;
	if (slot < 0 || slot >= ISDN_MAX_CHANNELS) {
		printk(KERN_ERR "isdn_ppp_push_higher: lp->ppp_slot(%d)\n",
		       lp->ppp_slot);
		goto drop_packet;
	}
	is = ippp_table[slot];

	if (lp->master) { // FIXME?
		mlp = ISDN_MASTER_PRIV(lp);
		slot = mlp->ppp_slot;
		if (slot < 0 || slot >= ISDN_MAX_CHANNELS) {
			printk(KERN_ERR "isdn_ppp_push_higher: master->ppp_slot(%d)\n",
			       lp->ppp_slot);
			goto drop_packet;
		}
	}
	mis = ippp_table[slot];

	if (is->debug & 0x10) {
		printk(KERN_DEBUG "push, skb %d %04x\n", (int) skb->len, proto);
		isdn_ppp_frame_log("rpush", skb->data, skb->len, 32, is->unit, lp->ppp_slot);
	}
	if (mis->compflags & SC_DECOMP_ON) {
		skb = isdn_ppp_decompress(skb, is, mis, &proto);
		if (!skb) // decompression error
			return;
	}
	switch (proto) {
	case PPP_IPX:  /* untested */
		if (is->debug & 0x20)
			printk(KERN_DEBUG "isdn_ppp: IPX\n");
		skb->protocol = htons(ETH_P_IPX);
		break;
	case PPP_IP:
		if (is->debug & 0x20)
			printk(KERN_DEBUG "isdn_ppp: IP\n");
		skb->protocol = htons(ETH_P_IP);
		break;
	case PPP_COMP:
	case PPP_COMPFRAG:
		printk(KERN_INFO "isdn_ppp: unexpected compressed frame dropped\n");
		goto drop_packet;
#ifdef CONFIG_ISDN_PPP_VJ
	case PPP_VJC_UNCOMP:
		if (is->debug & 0x20)
			printk(KERN_DEBUG "isdn_ppp: VJC_UNCOMP\n");
		if (net_dev->local->ppp_slot < 0) {
			printk(KERN_ERR "%s: net_dev->local->ppp_slot(%d) out of range\n",
			       __func__, net_dev->local->ppp_slot);
			goto drop_packet;
		}
		if (slhc_remember(ippp_table[net_dev->local->ppp_slot]->slcomp, skb->data, skb->len) <= 0) {
			printk(KERN_WARNING "isdn_ppp: received illegal VJC_UNCOMP frame!\n");
			goto drop_packet;
		}
		skb->protocol = htons(ETH_P_IP);
		break;
	case PPP_VJC_COMP:
		if (is->debug & 0x20)
			printk(KERN_DEBUG "isdn_ppp: VJC_COMP\n");
		{
			struct sk_buff *skb_old = skb;
			int pkt_len;
			skb = dev_alloc_skb(skb_old->len + 128);

			if (!skb) {
				printk(KERN_WARNING "%s: Memory squeeze, dropping packet.\n", dev->name);
				skb = skb_old;
				goto drop_packet;
			}
			skb_put(skb, skb_old->len + 128);
			skb_copy_from_linear_data(skb_old, skb->data,
						  skb_old->len);
			if (net_dev->local->ppp_slot < 0) {
				printk(KERN_ERR "%s: net_dev->local->ppp_slot(%d) out of range\n",
				       __func__, net_dev->local->ppp_slot);
				goto drop_packet;
			}
			pkt_len = slhc_uncompress(ippp_table[net_dev->local->ppp_slot]->slcomp,
						  skb->data, skb_old->len);
			kfree_skb(skb_old);
			if (pkt_len < 0)
				goto drop_packet;

			skb_trim(skb, pkt_len);
			skb->protocol = htons(ETH_P_IP);
		}
		break;
#endif
	case PPP_CCP:
	case PPP_CCPFRAG:
		isdn_ppp_receive_ccp(net_dev, lp, skb, proto);
		/* Dont pop up ResetReq/Ack stuff to the daemon any
		   longer - the job is done already */
		if (skb->data[0] == CCP_RESETREQ ||
		    skb->data[0] == CCP_RESETACK)
			break;
		/* fall through */
	default:
		isdn_ppp_fill_rq(skb->data, skb->len, proto, lp->ppp_slot);	/* push data to pppd device */
		kfree_skb(skb);
		return;
	}

#ifdef CONFIG_IPPP_FILTER
	/* check if the packet passes the pass and active filters
	 * the filter instructions are constructed assuming
	 * a four-byte PPP header on each packet (which is still present) */
	skb_push(skb, 4);

	{
		u_int16_t *p = (u_int16_t *) skb->data;

		*p = 0;	/* indicate inbound */
	}

	if (is->pass_filter
	    && BPF_PROG_RUN(is->pass_filter, skb) == 0) {
		if (is->debug & 0x2)
			printk(KERN_DEBUG "IPPP: inbound frame filtered.\n");
		kfree_skb(skb);
		return;
	}
	if (!(is->active_filter
	      && BPF_PROG_RUN(is->active_filter, skb) == 0)) {
		if (is->debug & 0x2)
			printk(KERN_DEBUG "IPPP: link-active filter: resetting huptimer.\n");
		lp->huptimer = 0;
		if (mlp)
			mlp->huptimer = 0;
	}
	skb_pull(skb, 4);
#else /* CONFIG_IPPP_FILTER */
	lp->huptimer = 0;
	if (mlp)
		mlp->huptimer = 0;
#endif /* CONFIG_IPPP_FILTER */
	skb->dev = dev;
	skb_reset_mac_header(skb);
	netif_rx(skb);
	/* net_dev->local->stats.rx_packets++; done in isdn_net.c */
	return;

drop_packet:
	net_dev->local->stats.rx_dropped++;
	kfree_skb(skb);
}