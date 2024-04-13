static struct dst_entry *ip6_route_input_lookup(struct net *net,
						struct net_device *dev,
						struct flowi6 *fl6, int flags)
{
	if (rt6_need_strict(&fl6->daddr) && dev->type != ARPHRD_PIMREG)
		flags |= RT6_LOOKUP_F_IFACE;

	return fib6_rule_lookup(net, fl6, flags, ip6_pol_route_input);
}