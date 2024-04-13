static struct sk_buff *isdn_ppp_compress(struct sk_buff *skb_in, int *proto,
					 struct ippp_struct *is, struct ippp_struct *master, int type)
{
	int ret;
	int new_proto;
	struct isdn_ppp_compressor *compressor;
	void *stat;
	struct sk_buff *skb_out;

	/* we do not compress control protocols */
	if (*proto < 0 || *proto > 0x3fff) {
		return skb_in;
	}

	if (type) { /* type=1 => Link compression */
		return skb_in;
	}
	else {
		if (!master) {
			compressor = is->compressor;
			stat = is->comp_stat;
		}
		else {
			compressor = master->compressor;
			stat = master->comp_stat;
		}
		new_proto = PPP_COMP;
	}

	if (!compressor) {
		printk(KERN_ERR "isdn_ppp: No compressor set!\n");
		return skb_in;
	}
	if (!stat) {
		printk(KERN_ERR "isdn_ppp: Compressor not initialized?\n");
		return skb_in;
	}

	/* Allow for at least 150 % expansion (for now) */
	skb_out = alloc_skb(skb_in->len + skb_in->len / 2 + 32 +
			    skb_headroom(skb_in), GFP_ATOMIC);
	if (!skb_out)
		return skb_in;
	skb_reserve(skb_out, skb_headroom(skb_in));

	ret = (compressor->compress)(stat, skb_in, skb_out, *proto);
	if (!ret) {
		dev_kfree_skb(skb_out);
		return skb_in;
	}

	dev_kfree_skb(skb_in);
	*proto = new_proto;
	return skb_out;
}