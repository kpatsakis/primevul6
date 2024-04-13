static void rt6_probe(struct rt6_info *rt)
{
	struct neighbour *neigh;
	/*
	 * Okay, this does not seem to be appropriate
	 * for now, however, we need to check if it
	 * is really so; aka Router Reachability Probing.
	 *
	 * Router Reachability Probe MUST be rate-limited
	 * to no more than one per minute.
	 */
	neigh = rt ? rt->n : NULL;
	if (!neigh || (neigh->nud_state & NUD_VALID))
		return;
	read_lock_bh(&neigh->lock);
	if (!(neigh->nud_state & NUD_VALID) &&
	    time_after(jiffies, neigh->updated + rt->rt6i_idev->cnf.rtr_probe_interval)) {
		struct in6_addr mcaddr;
		struct in6_addr *target;

		neigh->updated = jiffies;
		read_unlock_bh(&neigh->lock);

		target = (struct in6_addr *)&neigh->primary_key;
		addrconf_addr_solict_mult(target, &mcaddr);
		ndisc_send_ns(rt->dst.dev, NULL, target, &mcaddr, NULL);
	} else {
		read_unlock_bh(&neigh->lock);
	}
}