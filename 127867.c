ppp_receive_frame(struct ppp *ppp, struct sk_buff *skb, struct channel *pch)
{
	/* note: a 0-length skb is used as an error indication */
	if (skb->len > 0) {
		skb_checksum_complete_unset(skb);
#ifdef CONFIG_PPP_MULTILINK
		/* XXX do channel-level decompression here */
		if (PPP_PROTO(skb) == PPP_MP)
			ppp_receive_mp_frame(ppp, skb, pch);
		else
#endif /* CONFIG_PPP_MULTILINK */
			ppp_receive_nonmp_frame(ppp, skb);
	} else {
		kfree_skb(skb);
		ppp_receive_error(ppp);
	}
}