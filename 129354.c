static int rt6_mtu_change_route(struct rt6_info *rt, void *p_arg)
{
	struct rt6_mtu_change_arg *arg = (struct rt6_mtu_change_arg *) p_arg;
	struct inet6_dev *idev;

	/* In IPv6 pmtu discovery is not optional,
	   so that RTAX_MTU lock cannot disable it.
	   We still use this lock to block changes
	   caused by addrconf/ndisc.
	*/

	idev = __in6_dev_get(arg->dev);
	if (!idev)
		return 0;

	/* For administrative MTU increase, there is no way to discover
	   IPv6 PMTU increase, so PMTU increase should be updated here.
	   Since RFC 1981 doesn't include administrative MTU increase
	   update PMTU increase is a MUST. (i.e. jumbo frame)
	 */
	/*
	   If new MTU is less than route PMTU, this new MTU will be the
	   lowest MTU in the path, update the route PMTU to reflect PMTU
	   decreases; if new MTU is greater than route PMTU, and the
	   old MTU is the lowest MTU in the path, update the route PMTU
	   to reflect the increase. In this case if the other nodes' MTU
	   also have the lowest MTU, TOO BIG MESSAGE will be lead to
	   PMTU discouvery.
	 */
	if (rt->dst.dev == arg->dev &&
	    !dst_metric_locked(&rt->dst, RTAX_MTU) &&
	    (dst_mtu(&rt->dst) >= arg->mtu ||
	     (dst_mtu(&rt->dst) < arg->mtu &&
	      dst_mtu(&rt->dst) == idev->cnf.mtu6))) {
		dst_metric_set(&rt->dst, RTAX_MTU, arg->mtu);
	}
	return 0;
}