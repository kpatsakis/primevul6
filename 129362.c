static int ip6_pkt_prohibit(struct sk_buff *skb)
{
	return ip6_pkt_drop(skb, ICMPV6_ADM_PROHIBITED, IPSTATS_MIB_INNOROUTES);
}