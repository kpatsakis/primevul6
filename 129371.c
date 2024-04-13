static int __net_init ip6_route_net_init(struct net *net)
{
	int ret = -ENOMEM;

	memcpy(&net->ipv6.ip6_dst_ops, &ip6_dst_ops_template,
	       sizeof(net->ipv6.ip6_dst_ops));

	if (dst_entries_init(&net->ipv6.ip6_dst_ops) < 0)
		goto out_ip6_dst_ops;

	net->ipv6.ip6_null_entry = kmemdup(&ip6_null_entry_template,
					   sizeof(*net->ipv6.ip6_null_entry),
					   GFP_KERNEL);
	if (!net->ipv6.ip6_null_entry)
		goto out_ip6_dst_entries;
	net->ipv6.ip6_null_entry->dst.path =
		(struct dst_entry *)net->ipv6.ip6_null_entry;
	net->ipv6.ip6_null_entry->dst.ops = &net->ipv6.ip6_dst_ops;
	dst_init_metrics(&net->ipv6.ip6_null_entry->dst,
			 ip6_template_metrics, true);

#ifdef CONFIG_IPV6_MULTIPLE_TABLES
	net->ipv6.ip6_prohibit_entry = kmemdup(&ip6_prohibit_entry_template,
					       sizeof(*net->ipv6.ip6_prohibit_entry),
					       GFP_KERNEL);
	if (!net->ipv6.ip6_prohibit_entry)
		goto out_ip6_null_entry;
	net->ipv6.ip6_prohibit_entry->dst.path =
		(struct dst_entry *)net->ipv6.ip6_prohibit_entry;
	net->ipv6.ip6_prohibit_entry->dst.ops = &net->ipv6.ip6_dst_ops;
	dst_init_metrics(&net->ipv6.ip6_prohibit_entry->dst,
			 ip6_template_metrics, true);

	net->ipv6.ip6_blk_hole_entry = kmemdup(&ip6_blk_hole_entry_template,
					       sizeof(*net->ipv6.ip6_blk_hole_entry),
					       GFP_KERNEL);
	if (!net->ipv6.ip6_blk_hole_entry)
		goto out_ip6_prohibit_entry;
	net->ipv6.ip6_blk_hole_entry->dst.path =
		(struct dst_entry *)net->ipv6.ip6_blk_hole_entry;
	net->ipv6.ip6_blk_hole_entry->dst.ops = &net->ipv6.ip6_dst_ops;
	dst_init_metrics(&net->ipv6.ip6_blk_hole_entry->dst,
			 ip6_template_metrics, true);
#endif

	net->ipv6.sysctl.flush_delay = 0;
	net->ipv6.sysctl.ip6_rt_max_size = 4096;
	net->ipv6.sysctl.ip6_rt_gc_min_interval = HZ / 2;
	net->ipv6.sysctl.ip6_rt_gc_timeout = 60*HZ;
	net->ipv6.sysctl.ip6_rt_gc_interval = 30*HZ;
	net->ipv6.sysctl.ip6_rt_gc_elasticity = 9;
	net->ipv6.sysctl.ip6_rt_mtu_expires = 10*60*HZ;
	net->ipv6.sysctl.ip6_rt_min_advmss = IPV6_MIN_MTU - 20 - 40;

	net->ipv6.ip6_rt_gc_expire = 30*HZ;

	ret = 0;
out:
	return ret;

#ifdef CONFIG_IPV6_MULTIPLE_TABLES
out_ip6_prohibit_entry:
	kfree(net->ipv6.ip6_prohibit_entry);
out_ip6_null_entry:
	kfree(net->ipv6.ip6_null_entry);
#endif
out_ip6_dst_entries:
	dst_entries_destroy(&net->ipv6.ip6_dst_ops);
out_ip6_dst_ops:
	goto out;
}