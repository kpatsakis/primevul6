static void isdn_ppp_receive_ccp(isdn_net_dev *net_dev, isdn_net_local *lp,
				 struct sk_buff *skb, int proto)
{
	struct ippp_struct *is;
	struct ippp_struct *mis;
	int len;
	struct isdn_ppp_resetparams rsparm;
	unsigned char rsdata[IPPP_RESET_MAXDATABYTES];

	printk(KERN_DEBUG "Received CCP frame from peer slot(%d)\n",
	       lp->ppp_slot);
	if (lp->ppp_slot < 0 || lp->ppp_slot >= ISDN_MAX_CHANNELS) {
		printk(KERN_ERR "%s: lp->ppp_slot(%d) out of range\n",
		       __func__, lp->ppp_slot);
		return;
	}
	is = ippp_table[lp->ppp_slot];
	isdn_ppp_frame_log("ccp-rcv", skb->data, skb->len, 32, is->unit, lp->ppp_slot);

	if (lp->master) {
		int slot = ISDN_MASTER_PRIV(lp)->ppp_slot;
		if (slot < 0 || slot >= ISDN_MAX_CHANNELS) {
			printk(KERN_ERR "%s: slot(%d) out of range\n",
			       __func__, slot);
			return;
		}
		mis = ippp_table[slot];
	} else
		mis = is;

	switch (skb->data[0]) {
	case CCP_CONFREQ:
		if (is->debug & 0x10)
			printk(KERN_DEBUG "Disable compression here!\n");
		if (proto == PPP_CCP)
			mis->compflags &= ~SC_COMP_ON;
		else
			is->compflags &= ~SC_LINK_COMP_ON;
		break;
	case CCP_TERMREQ:
	case CCP_TERMACK:
		if (is->debug & 0x10)
			printk(KERN_DEBUG "Disable (de)compression here!\n");
		if (proto == PPP_CCP)
			mis->compflags &= ~(SC_DECOMP_ON | SC_COMP_ON);
		else
			is->compflags &= ~(SC_LINK_DECOMP_ON | SC_LINK_COMP_ON);
		break;
	case CCP_CONFACK:
		/* if we RECEIVE an ackowledge we enable the decompressor */
		if (is->debug & 0x10)
			printk(KERN_DEBUG "Enable decompression here!\n");
		if (proto == PPP_CCP) {
			if (!mis->decompressor)
				break;
			mis->compflags |= SC_DECOMP_ON;
		} else {
			if (!is->decompressor)
				break;
			is->compflags |= SC_LINK_DECOMP_ON;
		}
		break;

	case CCP_RESETACK:
		printk(KERN_DEBUG "Received ResetAck from peer\n");
		len = (skb->data[2] << 8) | skb->data[3];
		len -= 4;

		if (proto == PPP_CCP) {
			/* If a reset Ack was outstanding for this id, then
			   clean up the state engine */
			isdn_ppp_ccp_reset_ack_rcvd(mis, skb->data[1]);
			if (mis->decompressor && mis->decomp_stat)
				mis->decompressor->
					reset(mis->decomp_stat,
					      skb->data[0],
					      skb->data[1],
					      len ? &skb->data[4] : NULL,
					      len, NULL);
			/* TODO: This is not easy to decide here */
			mis->compflags &= ~SC_DECOMP_DISCARD;
		}
		else {
			isdn_ppp_ccp_reset_ack_rcvd(is, skb->data[1]);
			if (is->link_decompressor && is->link_decomp_stat)
				is->link_decompressor->
					reset(is->link_decomp_stat,
					      skb->data[0],
					      skb->data[1],
					      len ? &skb->data[4] : NULL,
					      len, NULL);
			/* TODO: neither here */
			is->compflags &= ~SC_LINK_DECOMP_DISCARD;
		}
		break;

	case CCP_RESETREQ:
		printk(KERN_DEBUG "Received ResetReq from peer\n");
		/* Receiving a ResetReq means we must reset our compressor */
		/* Set up reset params for the reset entry */
		memset(&rsparm, 0, sizeof(rsparm));
		rsparm.data = rsdata;
		rsparm.maxdlen = IPPP_RESET_MAXDATABYTES;
		/* Isolate data length */
		len = (skb->data[2] << 8) | skb->data[3];
		len -= 4;
		if (proto == PPP_CCP) {
			if (mis->compressor && mis->comp_stat)
				mis->compressor->
					reset(mis->comp_stat,
					      skb->data[0],
					      skb->data[1],
					      len ? &skb->data[4] : NULL,
					      len, &rsparm);
		}
		else {
			if (is->link_compressor && is->link_comp_stat)
				is->link_compressor->
					reset(is->link_comp_stat,
					      skb->data[0],
					      skb->data[1],
					      len ? &skb->data[4] : NULL,
					      len, &rsparm);
		}
		/* Ack the Req as specified by rsparm */
		if (rsparm.valid) {
			/* Compressor reset handler decided how to answer */
			if (rsparm.rsend) {
				/* We should send a Frame */
				isdn_ppp_ccp_xmit_reset(is, proto, CCP_RESETACK,
							rsparm.idval ? rsparm.id
							: skb->data[1],
							rsparm.dtval ?
							rsparm.data : NULL,
							rsparm.dtval ?
							rsparm.dlen : 0);
			} else {
				printk(KERN_DEBUG "ResetAck suppressed\n");
			}
		} else {
			/* We answer with a straight reflected Ack */
			isdn_ppp_ccp_xmit_reset(is, proto, CCP_RESETACK,
						skb->data[1],
						len ? &skb->data[4] : NULL,
						len);
		}
		break;
	}
}