struct rt6_info *rt6_add_dflt_router(const struct in6_addr *gwaddr,
				     struct net_device *dev,
				     unsigned int pref)
{
	struct fib6_config cfg = {
		.fc_table	= RT6_TABLE_DFLT,
		.fc_metric	= IP6_RT_PRIO_USER,
		.fc_ifindex	= dev->ifindex,
		.fc_flags	= RTF_GATEWAY | RTF_ADDRCONF | RTF_DEFAULT |
				  RTF_UP | RTF_EXPIRES | RTF_PREF(pref),
		.fc_nlinfo.portid = 0,
		.fc_nlinfo.nlh = NULL,
		.fc_nlinfo.nl_net = dev_net(dev),
	};

	cfg.fc_gateway = *gwaddr;

	ip6_route_add(&cfg);

	return rt6_get_dflt_router(gwaddr, dev);
}