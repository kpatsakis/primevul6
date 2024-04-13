isdn_ppp_xmit(struct sk_buff *skb, struct net_device *netdev)
{
	isdn_net_local *lp, *mlp;
	isdn_net_dev *nd;
	unsigned int proto = PPP_IP;     /* 0x21 */
	struct ippp_struct *ipt, *ipts;
	int slot, retval = NETDEV_TX_OK;

	mlp = netdev_priv(netdev);
	nd = mlp->netdev;       /* get master lp */

	slot = mlp->ppp_slot;
	if (slot < 0 || slot >= ISDN_MAX_CHANNELS) {
		printk(KERN_ERR "isdn_ppp_xmit: lp->ppp_slot(%d)\n",
		       mlp->ppp_slot);
		kfree_skb(skb);
		goto out;
	}
	ipts = ippp_table[slot];

	if (!(ipts->pppcfg & SC_ENABLE_IP)) {	/* PPP connected ? */
		if (ipts->debug & 0x1)
			printk(KERN_INFO "%s: IP frame delayed.\n", netdev->name);
		retval = NETDEV_TX_BUSY;
		goto out;
	}

	switch (ntohs(skb->protocol)) {
	case ETH_P_IP:
		proto = PPP_IP;
		break;
	case ETH_P_IPX:
		proto = PPP_IPX;	/* untested */
		break;
	default:
		printk(KERN_ERR "isdn_ppp: skipped unsupported protocol: %#x.\n",
		       skb->protocol);
		dev_kfree_skb(skb);
		goto out;
	}

	lp = isdn_net_get_locked_lp(nd);
	if (!lp) {
		printk(KERN_WARNING "%s: all channels busy - requeuing!\n", netdev->name);
		retval = NETDEV_TX_BUSY;
		goto out;
	}
	/* we have our lp locked from now on */

	slot = lp->ppp_slot;
	if (slot < 0 || slot >= ISDN_MAX_CHANNELS) {
		printk(KERN_ERR "isdn_ppp_xmit: lp->ppp_slot(%d)\n",
		       lp->ppp_slot);
		kfree_skb(skb);
		goto unlock;
	}
	ipt = ippp_table[slot];

	/*
	 * after this line .. requeueing in the device queue is no longer allowed!!!
	 */

	/* Pull off the fake header we stuck on earlier to keep
	 * the fragmentation code happy.
	 */
	skb_pull(skb, IPPP_MAX_HEADER);

#ifdef CONFIG_IPPP_FILTER
	/* check if we should pass this packet
	 * the filter instructions are constructed assuming
	 * a four-byte PPP header on each packet */
	*skb_push(skb, 4) = 1; /* indicate outbound */

	{
		__be16 *p = (__be16 *)skb->data;

		p++;
		*p = htons(proto);
	}

	if (ipt->pass_filter
	    && BPF_PROG_RUN(ipt->pass_filter, skb) == 0) {
		if (ipt->debug & 0x4)
			printk(KERN_DEBUG "IPPP: outbound frame filtered.\n");
		kfree_skb(skb);
		goto unlock;
	}
	if (!(ipt->active_filter
	      && BPF_PROG_RUN(ipt->active_filter, skb) == 0)) {
		if (ipt->debug & 0x4)
			printk(KERN_DEBUG "IPPP: link-active filter: resetting huptimer.\n");
		lp->huptimer = 0;
	}
	skb_pull(skb, 4);
#else /* CONFIG_IPPP_FILTER */
	lp->huptimer = 0;
#endif /* CONFIG_IPPP_FILTER */

	if (ipt->debug & 0x4)
		printk(KERN_DEBUG "xmit skb, len %d\n", (int) skb->len);
	if (ipts->debug & 0x40)
		isdn_ppp_frame_log("xmit0", skb->data, skb->len, 32, ipts->unit, lp->ppp_slot);

#ifdef CONFIG_ISDN_PPP_VJ
	if (proto == PPP_IP && ipts->pppcfg & SC_COMP_TCP) {	/* ipts here? probably yes, but check this again */
		struct sk_buff *new_skb;
		unsigned short hl;
		/*
		 * we need to reserve enough space in front of
		 * sk_buff. old call to dev_alloc_skb only reserved
		 * 16 bytes, now we are looking what the driver want.
		 */
		hl = dev->drv[lp->isdn_device]->interface->hl_hdrlen + IPPP_MAX_HEADER;
		/*
		 * Note: hl might still be insufficient because the method
		 * above does not account for a possibible MPPP slave channel
		 * which had larger HL header space requirements than the
		 * master.
		 */
		new_skb = alloc_skb(hl + skb->len, GFP_ATOMIC);
		if (new_skb) {
			u_char *buf;
			int pktlen;

			skb_reserve(new_skb, hl);
			new_skb->dev = skb->dev;
			skb_put(new_skb, skb->len);
			buf = skb->data;

			pktlen = slhc_compress(ipts->slcomp, skb->data, skb->len, new_skb->data,
					       &buf, !(ipts->pppcfg & SC_NO_TCP_CCID));

			if (buf != skb->data) {
				if (new_skb->data != buf)
					printk(KERN_ERR "isdn_ppp: FATAL error after slhc_compress!!\n");
				dev_kfree_skb(skb);
				skb = new_skb;
			} else {
				dev_kfree_skb(new_skb);
			}

			skb_trim(skb, pktlen);
			if (skb->data[0] & SL_TYPE_COMPRESSED_TCP) {	/* cslip? style -> PPP */
				proto = PPP_VJC_COMP;
				skb->data[0] ^= SL_TYPE_COMPRESSED_TCP;
			} else {
				if (skb->data[0] >= SL_TYPE_UNCOMPRESSED_TCP)
					proto = PPP_VJC_UNCOMP;
				skb->data[0] = (skb->data[0] & 0x0f) | 0x40;
			}
		}
	}
#endif

	/*
	 * normal (single link) or bundle compression
	 */
	if (ipts->compflags & SC_COMP_ON) {
		/* We send compressed only if both down- und upstream
		   compression is negotiated, that means, CCP is up */
		if (ipts->compflags & SC_DECOMP_ON) {
			skb = isdn_ppp_compress(skb, &proto, ipt, ipts, 0);
		} else {
			printk(KERN_DEBUG "isdn_ppp: CCP not yet up - sending as-is\n");
		}
	}

	if (ipt->debug & 0x24)
		printk(KERN_DEBUG "xmit2 skb, len %d, proto %04x\n", (int) skb->len, proto);

#ifdef CONFIG_ISDN_MPP
	if (ipt->mpppcfg & SC_MP_PROT) {
		/* we get mp_seqno from static isdn_net_local */
		long mp_seqno = ipts->mp_seqno;
		ipts->mp_seqno++;
		if (ipt->mpppcfg & SC_OUT_SHORT_SEQ) {
			unsigned char *data = isdn_ppp_skb_push(&skb, 3);
			if (!data)
				goto unlock;
			mp_seqno &= 0xfff;
			data[0] = MP_BEGIN_FRAG | MP_END_FRAG | ((mp_seqno >> 8) & 0xf);	/* (B)egin & (E)ndbit .. */
			data[1] = mp_seqno & 0xff;
			data[2] = proto;	/* PID compression */
		} else {
			unsigned char *data = isdn_ppp_skb_push(&skb, 5);
			if (!data)
				goto unlock;
			data[0] = MP_BEGIN_FRAG | MP_END_FRAG;	/* (B)egin & (E)ndbit .. */
			data[1] = (mp_seqno >> 16) & 0xff;	/* sequence number: 24bit */
			data[2] = (mp_seqno >> 8) & 0xff;
			data[3] = (mp_seqno >> 0) & 0xff;
			data[4] = proto;	/* PID compression */
		}
		proto = PPP_MP; /* MP Protocol, 0x003d */
	}
#endif

	/*
	 * 'link in bundle' compression  ...
	 */
	if (ipt->compflags & SC_LINK_COMP_ON)
		skb = isdn_ppp_compress(skb, &proto, ipt, ipts, 1);

	if ((ipt->pppcfg & SC_COMP_PROT) && (proto <= 0xff)) {
		unsigned char *data = isdn_ppp_skb_push(&skb, 1);
		if (!data)
			goto unlock;
		data[0] = proto & 0xff;
	}
	else {
		unsigned char *data = isdn_ppp_skb_push(&skb, 2);
		if (!data)
			goto unlock;
		data[0] = (proto >> 8) & 0xff;
		data[1] = proto & 0xff;
	}
	if (!(ipt->pppcfg & SC_COMP_AC)) {
		unsigned char *data = isdn_ppp_skb_push(&skb, 2);
		if (!data)
			goto unlock;
		data[0] = 0xff;    /* All Stations */
		data[1] = 0x03;    /* Unnumbered information */
	}

	/* tx-stats are now updated via BSENT-callback */

	if (ipts->debug & 0x40) {
		printk(KERN_DEBUG "skb xmit: len: %d\n", (int) skb->len);
		isdn_ppp_frame_log("xmit", skb->data, skb->len, 32, ipt->unit, lp->ppp_slot);
	}

	isdn_net_writebuf_skb(lp, skb);

unlock:
	spin_unlock_bh(&lp->xmit_lock);
out:
	return retval;
}