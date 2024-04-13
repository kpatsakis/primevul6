static inline struct rt6_info *rt6_device_match(struct net *net,
						    struct rt6_info *rt,
						    const struct in6_addr *saddr,
						    int oif,
						    int flags)
{
	struct rt6_info *local = NULL;
	struct rt6_info *sprt;

	if (!oif && ipv6_addr_any(saddr))
		goto out;

	for (sprt = rt; sprt; sprt = sprt->dst.rt6_next) {
		struct net_device *dev = sprt->dst.dev;

		if (oif) {
			if (dev->ifindex == oif)
				return sprt;
			if (dev->flags & IFF_LOOPBACK) {
				if (!sprt->rt6i_idev ||
				    sprt->rt6i_idev->dev->ifindex != oif) {
					if (flags & RT6_LOOKUP_F_IFACE && oif)
						continue;
					if (local && (!oif ||
						      local->rt6i_idev->dev->ifindex == oif))
						continue;
				}
				local = sprt;
			}
		} else {
			if (ipv6_chk_addr(net, saddr, dev,
					  flags & RT6_LOOKUP_F_IFACE))
				return sprt;
		}
	}

	if (oif) {
		if (local)
			return local;

		if (flags & RT6_LOOKUP_F_IFACE)
			return net->ipv6.ip6_null_entry;
	}
out:
	return rt;
}