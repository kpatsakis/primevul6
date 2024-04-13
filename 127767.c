struct sk_buff *isdn_ppp_mp_discard(ippp_bundle *mp,
				    struct sk_buff *from, struct sk_buff *to)
{
	if (from)
		while (from != to) {
			struct sk_buff *next = from->next;
			isdn_ppp_mp_free_skb(mp, from);
			from = next;
		}
	return from;
}