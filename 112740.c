syn_cache_lookup(struct sockaddr *src, struct sockaddr *dst,
    struct syn_cache_head **headp, u_int rtableid)
{
	struct syn_cache_set *sets[2];
	struct syn_cache *sc;
	struct syn_cache_head *scp;
	u_int32_t hash;
	int i;

	NET_ASSERT_LOCKED();

	/* Check the active cache first, the passive cache is likely emtpy. */
	sets[0] = &tcp_syn_cache[tcp_syn_cache_active];
	sets[1] = &tcp_syn_cache[!tcp_syn_cache_active];
	for (i = 0; i < 2; i++) {
		if (sets[i]->scs_count == 0)
			continue;
		SYN_HASHALL(hash, src, dst, sets[i]->scs_random);
		scp = &sets[i]->scs_buckethead[hash % sets[i]->scs_size];
		*headp = scp;
		TAILQ_FOREACH(sc, &scp->sch_bucket, sc_bucketq) {
			if (sc->sc_hash != hash)
				continue;
			if (!bcmp(&sc->sc_src, src, src->sa_len) &&
			    !bcmp(&sc->sc_dst, dst, dst->sa_len) &&
			    rtable_l2(rtableid) == rtable_l2(sc->sc_rtableid))
				return (sc);
		}
	}
	return (NULL);
}