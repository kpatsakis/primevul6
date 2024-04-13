static void isdn_ppp_mp_free_skb(ippp_bundle *mp, struct sk_buff *skb)
{
	dev_kfree_skb(skb);
	mp->frames--;
}