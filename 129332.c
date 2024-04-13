void ip6_update_pmtu(struct sk_buff *skb, struct net *net, __be32 mtu,
		     int oif, u32 mark)
{
	const struct ipv6hdr *iph = (struct ipv6hdr *) skb->data;
	struct dst_entry *dst;
	struct flowi6 fl6;

	memset(&fl6, 0, sizeof(fl6));
	fl6.flowi6_oif = oif;
	fl6.flowi6_mark = mark;
	fl6.flowi6_flags = 0;
	fl6.daddr = iph->daddr;
	fl6.saddr = iph->saddr;
	fl6.flowlabel = (*(__be32 *) iph) & IPV6_FLOWINFO_MASK;

	dst = ip6_route_output(net, NULL, &fl6);
	if (!dst->error)
		ip6_rt_update_pmtu(dst, NULL, skb, ntohl(mtu));
	dst_release(dst);
}