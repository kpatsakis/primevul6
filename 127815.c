void isdn_ppp_mp_reassembly(isdn_net_dev *net_dev, isdn_net_local *lp,
			    struct sk_buff *from, struct sk_buff *to)
{
	ippp_bundle *mp = net_dev->pb;
	int proto;
	struct sk_buff *skb;
	unsigned int tot_len;

	if (lp->ppp_slot < 0 || lp->ppp_slot >= ISDN_MAX_CHANNELS) {
		printk(KERN_ERR "%s: lp->ppp_slot(%d) out of range\n",
		       __func__, lp->ppp_slot);
		return;
	}
	if (MP_FLAGS(from) == (MP_BEGIN_FRAG | MP_END_FRAG)) {
		if (ippp_table[lp->ppp_slot]->debug & 0x40)
			printk(KERN_DEBUG "isdn_mppp: reassembly: frame %d, "
			       "len %d\n", MP_SEQ(from), from->len);
		skb = from;
		skb_pull(skb, MP_HEADER_LEN);
		mp->frames--;
	} else {
		struct sk_buff *frag;
		int n;

		for (tot_len = n = 0, frag = from; frag != to; frag = frag->next, n++)
			tot_len += frag->len - MP_HEADER_LEN;

		if (ippp_table[lp->ppp_slot]->debug & 0x40)
			printk(KERN_DEBUG"isdn_mppp: reassembling frames %d "
			       "to %d, len %d\n", MP_SEQ(from),
			       (MP_SEQ(from) + n - 1) & MP_LONGSEQ_MASK, tot_len);
		if ((skb = dev_alloc_skb(tot_len)) == NULL) {
			printk(KERN_ERR "isdn_mppp: cannot allocate sk buff "
			       "of size %d\n", tot_len);
			isdn_ppp_mp_discard(mp, from, to);
			return;
		}

		while (from != to) {
			unsigned int len = from->len - MP_HEADER_LEN;

			skb_copy_from_linear_data_offset(from, MP_HEADER_LEN,
							 skb_put(skb, len),
							 len);
			frag = from->next;
			isdn_ppp_mp_free_skb(mp, from);
			from = frag;
		}
	}
	proto = isdn_ppp_strip_proto(skb);
	isdn_ppp_push_higher(net_dev, lp, skb, proto);
}