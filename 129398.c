static inline const void *choose_neigh_daddr(struct rt6_info *rt,
					     struct sk_buff *skb,
					     const void *daddr)
{
	struct in6_addr *p = &rt->rt6i_gateway;

	if (!ipv6_addr_any(p))
		return (const void *) p;
	else if (skb)
		return &ipv6_hdr(skb)->daddr;
	return daddr;
}