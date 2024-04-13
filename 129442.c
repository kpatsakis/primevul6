struct rt6_info *rt6_lookup(struct net *net, const struct in6_addr *daddr,
			    const struct in6_addr *saddr, int oif, int strict)
{
	struct flowi6 fl6 = {
		.flowi6_oif = oif,
		.daddr = *daddr,
	};
	struct dst_entry *dst;
	int flags = strict ? RT6_LOOKUP_F_IFACE : 0;

	if (saddr) {
		memcpy(&fl6.saddr, saddr, sizeof(*saddr));
		flags |= RT6_LOOKUP_F_HAS_SADDR;
	}

	dst = fib6_rule_lookup(net, &fl6, flags, ip6_pol_route_lookup);
	if (dst->error == 0)
		return (struct rt6_info *) dst;

	dst_release(dst);

	return NULL;
}