static inline int rt6_check_neigh(struct rt6_info *rt)
{
	struct neighbour *neigh;
	int m;

	neigh = rt->n;
	if (rt->rt6i_flags & RTF_NONEXTHOP ||
	    !(rt->rt6i_flags & RTF_GATEWAY))
		m = 1;
	else if (neigh) {
		read_lock_bh(&neigh->lock);
		if (neigh->nud_state & NUD_VALID)
			m = 2;
#ifdef CONFIG_IPV6_ROUTER_PREF
		else if (neigh->nud_state & NUD_FAILED)
			m = 0;
#endif
		else
			m = 1;
		read_unlock_bh(&neigh->lock);
	} else
		m = 0;
	return m;
}