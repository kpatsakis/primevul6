static struct rt6_info *ip6_pol_route_output(struct net *net, struct fib6_table *table,
					     struct flowi6 *fl6, int flags)
{
	return ip6_pol_route(net, table, fl6->flowi6_oif, fl6, flags);
}