static struct rt6_info *rt6_alloc_cow(struct rt6_info *ort,
				      const struct in6_addr *daddr,
				      const struct in6_addr *saddr)
{
	struct rt6_info *rt;

	/*
	 *	Clone the route.
	 */

	rt = ip6_rt_copy(ort, daddr);

	if (rt) {
		int attempts = !in_softirq();

		if (!(rt->rt6i_flags & RTF_GATEWAY)) {
			if (ort->rt6i_dst.plen != 128 &&
			    ipv6_addr_equal(&ort->rt6i_dst.addr, daddr))
				rt->rt6i_flags |= RTF_ANYCAST;
			rt->rt6i_gateway = *daddr;
		}

		rt->rt6i_flags |= RTF_CACHE;

#ifdef CONFIG_IPV6_SUBTREES
		if (rt->rt6i_src.plen && saddr) {
			rt->rt6i_src.addr = *saddr;
			rt->rt6i_src.plen = 128;
		}
#endif

	retry:
		if (rt6_bind_neighbour(rt, rt->dst.dev)) {
			struct net *net = dev_net(rt->dst.dev);
			int saved_rt_min_interval =
				net->ipv6.sysctl.ip6_rt_gc_min_interval;
			int saved_rt_elasticity =
				net->ipv6.sysctl.ip6_rt_gc_elasticity;

			if (attempts-- > 0) {
				net->ipv6.sysctl.ip6_rt_gc_elasticity = 1;
				net->ipv6.sysctl.ip6_rt_gc_min_interval = 0;

				ip6_dst_gc(&net->ipv6.ip6_dst_ops);

				net->ipv6.sysctl.ip6_rt_gc_elasticity =
					saved_rt_elasticity;
				net->ipv6.sysctl.ip6_rt_gc_min_interval =
					saved_rt_min_interval;
				goto retry;
			}

			net_warn_ratelimited("Neighbour table overflow\n");
			dst_free(&rt->dst);
			return NULL;
		}
	}

	return rt;
}