int isdn_ppp_autodial_filter(struct sk_buff *skb, isdn_net_local *lp)
{
	struct ippp_struct *is = ippp_table[lp->ppp_slot];
	u_int16_t proto;
	int drop = 0;

	switch (ntohs(skb->protocol)) {
	case ETH_P_IP:
		proto = PPP_IP;
		break;
	case ETH_P_IPX:
		proto = PPP_IPX;
		break;
	default:
		printk(KERN_ERR "isdn_ppp_autodial_filter: unsupported protocol 0x%x.\n",
		       skb->protocol);
		return 1;
	}

	/* the filter instructions are constructed assuming
	 * a four-byte PPP header on each packet. we have to
	 * temporarily remove part of the fake header stuck on
	 * earlier.
	 */
	*skb_pull(skb, IPPP_MAX_HEADER - 4) = 1; /* indicate outbound */

	{
		__be16 *p = (__be16 *)skb->data;

		p++;
		*p = htons(proto);
	}

	drop |= is->pass_filter
		&& BPF_PROG_RUN(is->pass_filter, skb) == 0;
	drop |= is->active_filter
		&& BPF_PROG_RUN(is->active_filter, skb) == 0;

	skb_push(skb, IPPP_MAX_HEADER - 4);
	return drop;
}