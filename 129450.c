static int rt6_stats_seq_show(struct seq_file *seq, void *v)
{
	struct net *net = (struct net *)seq->private;
	seq_printf(seq, "%04x %04x %04x %04x %04x %04x %04x\n",
		   net->ipv6.rt6_stats->fib_nodes,
		   net->ipv6.rt6_stats->fib_route_nodes,
		   net->ipv6.rt6_stats->fib_rt_alloc,
		   net->ipv6.rt6_stats->fib_rt_entries,
		   net->ipv6.rt6_stats->fib_rt_cache,
		   dst_entries_get_slow(&net->ipv6.ip6_dst_ops),
		   net->ipv6.rt6_stats->fib_discarded_routes);

	return 0;
}