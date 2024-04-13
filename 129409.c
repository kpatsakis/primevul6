int ip6_del_rt(struct rt6_info *rt)
{
	struct nl_info info = {
		.nl_net = dev_net(rt->dst.dev),
	};
	return __ip6_del_rt(rt, &info);
}