static int rt6_info_route(struct rt6_info *rt, void *p_arg)
{
	struct seq_file *m = p_arg;
	struct neighbour *n;

	seq_printf(m, "%pi6 %02x ", &rt->rt6i_dst.addr, rt->rt6i_dst.plen);

#ifdef CONFIG_IPV6_SUBTREES
	seq_printf(m, "%pi6 %02x ", &rt->rt6i_src.addr, rt->rt6i_src.plen);
#else
	seq_puts(m, "00000000000000000000000000000000 00 ");
#endif
	n = rt->n;
	if (n) {
		seq_printf(m, "%pi6", n->primary_key);
	} else {
		seq_puts(m, "00000000000000000000000000000000");
	}
	seq_printf(m, " %08x %08x %08x %08x %8s\n",
		   rt->rt6i_metric, atomic_read(&rt->dst.__refcnt),
		   rt->dst.__use, rt->rt6i_flags,
		   rt->dst.dev ? rt->dst.dev->name : "");
	return 0;
}