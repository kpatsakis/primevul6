static struct smack_known *smack_from_skb(struct sk_buff *skb)
{
	if (skb == NULL || skb->secmark == 0)
		return NULL;

	return smack_from_secid(skb->secmark);
}