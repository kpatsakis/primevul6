static unsigned char *isdn_ppp_skb_push(struct sk_buff **skb_p, int len)
{
	struct sk_buff *skb = *skb_p;

	if (skb_headroom(skb) < len) {
		struct sk_buff *nskb = skb_realloc_headroom(skb, len);

		if (!nskb) {
			printk(KERN_ERR "isdn_ppp_skb_push: can't realloc headroom!\n");
			dev_kfree_skb(skb);
			return NULL;
		}
		printk(KERN_DEBUG "isdn_ppp_skb_push:under %d %d\n", skb_headroom(skb), len);
		dev_kfree_skb(skb);
		*skb_p = nskb;
		return skb_push(nskb, len);
	}
	return skb_push(skb, len);
}