static struct rt6_info *rt6_add_route_info(struct net *net,
					   const struct in6_addr *prefix, int prefixlen,
					   const struct in6_addr *gwaddr, int ifindex,
					   unsigned int pref)
{
	struct fib6_config cfg = {
		.fc_table	= RT6_TABLE_INFO,
		.fc_metric	= IP6_RT_PRIO_USER,
		.fc_ifindex	= ifindex,
		.fc_dst_len	= prefixlen,
		.fc_flags	= RTF_GATEWAY | RTF_ADDRCONF | RTF_ROUTEINFO |
				  RTF_UP | RTF_PREF(pref),
		.fc_nlinfo.portid = 0,
		.fc_nlinfo.nlh = NULL,
		.fc_nlinfo.nl_net = net,
	};

	cfg.fc_dst = *prefix;
	cfg.fc_gateway = *gwaddr;

	/* We should treat it as a default route if prefix length is 0. */
	if (!prefixlen)
		cfg.fc_flags |= RTF_DEFAULT;

	ip6_route_add(&cfg);

	return rt6_get_route_info(net, prefix, prefixlen, gwaddr, ifindex);
}