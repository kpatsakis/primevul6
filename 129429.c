static int fib6_remove_prefsrc(struct rt6_info *rt, void *arg)
{
	struct net_device *dev = ((struct arg_dev_net_ip *)arg)->dev;
	struct net *net = ((struct arg_dev_net_ip *)arg)->net;
	struct in6_addr *addr = ((struct arg_dev_net_ip *)arg)->addr;

	if (((void *)rt->dst.dev == dev || !dev) &&
	    rt != net->ipv6.ip6_null_entry &&
	    ipv6_addr_equal(addr, &rt->rt6i_prefsrc.addr)) {
		/* remove prefsrc entry */
		rt->rt6i_prefsrc.plen = 0;
	}
	return 0;
}