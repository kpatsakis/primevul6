static void __net_exit ip6_route_net_exit_late(struct net *net)
{
#ifdef CONFIG_PROC_FS
	proc_net_remove(net, "ipv6_route");
	proc_net_remove(net, "rt6_stats");
#endif
}