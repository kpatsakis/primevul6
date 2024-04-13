static void __net_exit ip6_route_net_exit(struct net *net)
{
	kfree(net->ipv6.ip6_null_entry);
#ifdef CONFIG_IPV6_MULTIPLE_TABLES
	kfree(net->ipv6.ip6_prohibit_entry);
	kfree(net->ipv6.ip6_blk_hole_entry);
#endif
	dst_entries_destroy(&net->ipv6.ip6_dst_ops);
}