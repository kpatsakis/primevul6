static struct sk_buff *isdn_ppp_decompress(struct sk_buff *skb, struct ippp_struct *is, struct ippp_struct *master,
					   int *proto)
{
	void *stat = NULL;
	struct isdn_ppp_compressor *ipc = NULL;
	struct sk_buff *skb_out;
	int len;
	struct ippp_struct *ri;
	struct isdn_ppp_resetparams rsparm;
	unsigned char rsdata[IPPP_RESET_MAXDATABYTES];

	if (!master) {
		// per-link decompression
		stat = is->link_decomp_stat;
		ipc = is->link_decompressor;
		ri = is;
	} else {
		stat = master->decomp_stat;
		ipc = master->decompressor;
		ri = master;
	}

	if (!ipc) {
		// no decompressor -> we can't decompress.
		printk(KERN_DEBUG "ippp: no decompressor defined!\n");
		return skb;
	}
	BUG_ON(!stat); // if we have a compressor, stat has been set as well

	if ((master && *proto == PPP_COMP) || (!master && *proto == PPP_COMPFRAG)) {
		// compressed packets are compressed by their protocol type

		// Set up reset params for the decompressor
		memset(&rsparm, 0, sizeof(rsparm));
		rsparm.data = rsdata;
		rsparm.maxdlen = IPPP_RESET_MAXDATABYTES;

		skb_out = dev_alloc_skb(is->mru + PPP_HDRLEN);
		if (!skb_out) {
			kfree_skb(skb);
			printk(KERN_ERR "ippp: decomp memory allocation failure\n");
			return NULL;
		}
		len = ipc->decompress(stat, skb, skb_out, &rsparm);
		kfree_skb(skb);
		if (len <= 0) {
			switch (len) {
			case DECOMP_ERROR:
				printk(KERN_INFO "ippp: decomp wants reset %s params\n",
				       rsparm.valid ? "with" : "without");

				isdn_ppp_ccp_reset_trans(ri, &rsparm);
				break;
			case DECOMP_FATALERROR:
				ri->pppcfg |= SC_DC_FERROR;
				/* Kick ipppd to recognize the error */
				isdn_ppp_ccp_kickup(ri);
				break;
			}
			kfree_skb(skb_out);
			return NULL;
		}
		*proto = isdn_ppp_strip_proto(skb_out);
		if (*proto < 0) {
			kfree_skb(skb_out);
			return NULL;
		}
		return skb_out;
	} else {
		// uncompressed packets are fed through the decompressor to
		// update the decompressor state
		ipc->incomp(stat, skb, *proto);
		return skb;
	}
}