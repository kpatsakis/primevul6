static void isdn_ppp_send_ccp(isdn_net_dev *net_dev, isdn_net_local *lp, struct sk_buff *skb)
{
	struct ippp_struct *mis, *is;
	int proto, slot = lp->ppp_slot;
	unsigned char *data;

	if (!skb || skb->len < 3)
		return;
	if (slot < 0 || slot >= ISDN_MAX_CHANNELS) {
		printk(KERN_ERR "%s: lp->ppp_slot(%d) out of range\n",
		       __func__, slot);
		return;
	}
	is = ippp_table[slot];
	/* Daemon may send with or without address and control field comp */
	data = skb->data;
	if (!(is->pppcfg & SC_COMP_AC) && data[0] == 0xff && data[1] == 0x03) {
		data += 2;
		if (skb->len < 5)
			return;
	}

	proto = ((int)data[0]<<8) + data[1];
	if (proto != PPP_CCP && proto != PPP_CCPFRAG)
		return;

	printk(KERN_DEBUG "Received CCP frame from daemon:\n");
	isdn_ppp_frame_log("ccp-xmit", skb->data, skb->len, 32, is->unit, lp->ppp_slot);

	if (lp->master) {
		slot = ISDN_MASTER_PRIV(lp)->ppp_slot;
		if (slot < 0 || slot >= ISDN_MAX_CHANNELS) {
			printk(KERN_ERR "%s: slot(%d) out of range\n",
			       __func__, slot);
			return;
		}
		mis = ippp_table[slot];
	} else
		mis = is;
	if (mis != is)
		printk(KERN_DEBUG "isdn_ppp: Ouch! Master CCP sends on slave slot!\n");

	switch (data[2]) {
	case CCP_CONFREQ:
		if (is->debug & 0x10)
			printk(KERN_DEBUG "Disable decompression here!\n");
		if (proto == PPP_CCP)
			is->compflags &= ~SC_DECOMP_ON;
		else
			is->compflags &= ~SC_LINK_DECOMP_ON;
		break;
	case CCP_TERMREQ:
	case CCP_TERMACK:
		if (is->debug & 0x10)
			printk(KERN_DEBUG "Disable (de)compression here!\n");
		if (proto == PPP_CCP)
			is->compflags &= ~(SC_DECOMP_ON | SC_COMP_ON);
		else
			is->compflags &= ~(SC_LINK_DECOMP_ON | SC_LINK_COMP_ON);
		break;
	case CCP_CONFACK:
		/* if we SEND an ackowledge we can/must enable the compressor */
		if (is->debug & 0x10)
			printk(KERN_DEBUG "Enable compression here!\n");
		if (proto == PPP_CCP) {
			if (!is->compressor)
				break;
			is->compflags |= SC_COMP_ON;
		} else {
			if (!is->compressor)
				break;
			is->compflags |= SC_LINK_COMP_ON;
		}
		break;
	case CCP_RESETACK:
		/* If we send a ACK we should reset our compressor */
		if (is->debug & 0x10)
			printk(KERN_DEBUG "Reset decompression state here!\n");
		printk(KERN_DEBUG "ResetAck from daemon passed by\n");
		if (proto == PPP_CCP) {
			/* link to master? */
			if (is->compressor && is->comp_stat)
				is->compressor->reset(is->comp_stat, 0, 0,
						      NULL, 0, NULL);
			is->compflags &= ~SC_COMP_DISCARD;
		}
		else {
			if (is->link_compressor && is->link_comp_stat)
				is->link_compressor->reset(is->link_comp_stat,
							   0, 0, NULL, 0, NULL);
			is->compflags &= ~SC_LINK_COMP_DISCARD;
		}
		break;
	case CCP_RESETREQ:
		/* Just let it pass by */
		printk(KERN_DEBUG "ResetReq from daemon passed by\n");
		break;
	}
}