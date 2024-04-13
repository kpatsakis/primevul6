static int ip6_pkt_discard(struct sk_buff *skb)
{
	return ip6_pkt_drop(skb, ICMPV6_NOROUTE, IPSTATS_MIB_INNOROUTES);
}