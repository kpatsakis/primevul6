static int __net_init ip6_route_net_init_late(struct net *net)
{
#ifdef CONFIG_PROC_FS
	proc_net_fops_create(net, "ipv6_route", 0, &ipv6_route_proc_fops);
	proc_net_fops_create(net, "rt6_stats", S_IRUGO, &rt6_stats_seq_fops);
#endif
	return 0;
}