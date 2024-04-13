static inline int rt6_check_dev(struct rt6_info *rt, int oif)
{
	struct net_device *dev = rt->dst.dev;
	if (!oif || dev->ifindex == oif)
		return 2;
	if ((dev->flags & IFF_LOOPBACK) &&
	    rt->rt6i_idev && rt->rt6i_idev->dev->ifindex == oif)
		return 1;
	return 0;
}