static int rt6_score_route(struct rt6_info *rt, int oif,
			   int strict)
{
	int m, n;

	m = rt6_check_dev(rt, oif);
	if (!m && (strict & RT6_LOOKUP_F_IFACE))
		return -1;
#ifdef CONFIG_IPV6_ROUTER_PREF
	m |= IPV6_DECODE_PREF(IPV6_EXTRACT_PREF(rt->rt6i_flags)) << 2;
#endif
	n = rt6_check_neigh(rt);
	if (!n && (strict & RT6_LOOKUP_F_REACHABLE))
		return -1;
	return m;
}