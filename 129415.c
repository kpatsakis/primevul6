static int ipv6_route_show(struct seq_file *m, void *v)
{
	struct net *net = (struct net *)m->private;
	fib6_clean_all_ro(net, rt6_info_route, 0, m);
	return 0;
}