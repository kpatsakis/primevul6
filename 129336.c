static void rtmsg_to_fib6_config(struct net *net,
				 struct in6_rtmsg *rtmsg,
				 struct fib6_config *cfg)
{
	memset(cfg, 0, sizeof(*cfg));

	cfg->fc_table = RT6_TABLE_MAIN;
	cfg->fc_ifindex = rtmsg->rtmsg_ifindex;
	cfg->fc_metric = rtmsg->rtmsg_metric;
	cfg->fc_expires = rtmsg->rtmsg_info;
	cfg->fc_dst_len = rtmsg->rtmsg_dst_len;
	cfg->fc_src_len = rtmsg->rtmsg_src_len;
	cfg->fc_flags = rtmsg->rtmsg_flags;

	cfg->fc_nlinfo.nl_net = net;

	cfg->fc_dst = rtmsg->rtmsg_dst;
	cfg->fc_src = rtmsg->rtmsg_src;
	cfg->fc_gateway = rtmsg->rtmsg_gateway;
}