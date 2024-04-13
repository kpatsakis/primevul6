static void isdn_ppp_mp_cleanup(isdn_net_local *lp)
{
	struct sk_buff *frag = lp->netdev->pb->frags;
	struct sk_buff *nextfrag;
	while (frag) {
		nextfrag = frag->next;
		isdn_ppp_mp_free_skb(lp->netdev->pb, frag);
		frag = nextfrag;
	}
	lp->netdev->pb->frags = NULL;
}