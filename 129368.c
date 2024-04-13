static int fib6_ifdown(struct rt6_info *rt, void *arg)
{
	const struct arg_dev_net *adn = arg;
	const struct net_device *dev = adn->dev;

	if ((rt->dst.dev == dev || !dev) &&
	    rt != adn->net->ipv6.ip6_null_entry)
		return -1;

	return 0;
}