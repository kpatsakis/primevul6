static struct dst_entry *ip6_dst_check(struct dst_entry *dst, u32 cookie)
{
	struct rt6_info *rt;

	rt = (struct rt6_info *) dst;

	/* All IPV6 dsts are created with ->obsolete set to the value
	 * DST_OBSOLETE_FORCE_CHK which forces validation calls down
	 * into this function always.
	 */
	if (rt->rt6i_genid != rt_genid(dev_net(rt->dst.dev)))
		return NULL;

	if (rt->rt6i_node && (rt->rt6i_node->fn_sernum == cookie)) {
		if (rt->rt6i_peer_genid != rt6_peer_genid()) {
			if (!rt6_has_peer(rt))
				rt6_bind_peer(rt, 0);
			rt->rt6i_peer_genid = rt6_peer_genid();
		}
		return dst;
	}
	return NULL;
}