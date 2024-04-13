static int isdn_ppp_strip_proto(struct sk_buff *skb)
{
	int proto;

	if (skb->len < 1)
		return -1;

	if (skb->data[0] & 0x1) {
		// protocol field is compressed
		proto = skb->data[0];
		skb_pull(skb, 1);
	} else {
		if (skb->len < 2)
			return -1;
		proto = ((int) skb->data[0] << 8) + skb->data[1];
		skb_pull(skb, 2);
	}
	return proto;
}