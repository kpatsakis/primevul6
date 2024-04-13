void ip6_route_cleanup(void)
{
	unregister_netdevice_notifier(&ip6_route_dev_notifier);
	unregister_pernet_subsys(&ip6_route_net_late_ops);
	fib6_rules_cleanup();
	xfrm6_fini();
	fib6_gc_cleanup();
	unregister_pernet_subsys(&ipv6_inetpeer_ops);
	unregister_pernet_subsys(&ip6_route_net_ops);
	dst_entries_destroy(&ip6_dst_blackhole_ops);
	kmem_cache_destroy(ip6_dst_ops_template.kmem_cachep);
}