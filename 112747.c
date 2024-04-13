syn_cache_insert(struct syn_cache *sc, struct tcpcb *tp)
{
	struct syn_cache_set *set = &tcp_syn_cache[tcp_syn_cache_active];
	struct syn_cache_head *scp;
	struct syn_cache *sc2;
	int i;

	NET_ASSERT_LOCKED();

	/*
	 * If there are no entries in the hash table, reinitialize
	 * the hash secrets.  To avoid useless cache swaps and
	 * reinitialization, use it until the limit is reached.
	 * An emtpy cache is also the oportunity to resize the hash.
	 */
	if (set->scs_count == 0 && set->scs_use <= 0) {
		set->scs_use = tcp_syn_use_limit;
		if (set->scs_size != tcp_syn_hash_size) {
			scp = mallocarray(tcp_syn_hash_size, sizeof(struct
			    syn_cache_head), M_SYNCACHE, M_NOWAIT|M_ZERO);
			if (scp == NULL) {
				/* Try again next time. */
				set->scs_use = 0;
			} else {
				free(set->scs_buckethead, M_SYNCACHE,
				    set->scs_size *
				    sizeof(struct syn_cache_head));
				set->scs_buckethead = scp;
				set->scs_size = tcp_syn_hash_size;
				for (i = 0; i < tcp_syn_hash_size; i++)
					TAILQ_INIT(&scp[i].sch_bucket);
			}
		}
		arc4random_buf(set->scs_random, sizeof(set->scs_random));
		tcpstat_inc(tcps_sc_seedrandom);
	}

	SYN_HASHALL(sc->sc_hash, &sc->sc_src.sa, &sc->sc_dst.sa,
	    set->scs_random);
	scp = &set->scs_buckethead[sc->sc_hash % set->scs_size];
	sc->sc_buckethead = scp;

	/*
	 * Make sure that we don't overflow the per-bucket
	 * limit or the total cache size limit.
	 */
	if (scp->sch_length >= tcp_syn_bucket_limit) {
		tcpstat_inc(tcps_sc_bucketoverflow);
		/*
		 * Someone might attack our bucket hash function.  Reseed
		 * with random as soon as the passive syn cache gets empty.
		 */
		set->scs_use = 0;
		/*
		 * The bucket is full.  Toss the oldest element in the
		 * bucket.  This will be the first entry in the bucket.
		 */
		sc2 = TAILQ_FIRST(&scp->sch_bucket);
#ifdef DIAGNOSTIC
		/*
		 * This should never happen; we should always find an
		 * entry in our bucket.
		 */
		if (sc2 == NULL)
			panic("%s: bucketoverflow: impossible", __func__);
#endif
		syn_cache_rm(sc2);
		syn_cache_put(sc2);
	} else if (set->scs_count >= tcp_syn_cache_limit) {
		struct syn_cache_head *scp2, *sce;

		tcpstat_inc(tcps_sc_overflowed);
		/*
		 * The cache is full.  Toss the oldest entry in the
		 * first non-empty bucket we can find.
		 *
		 * XXX We would really like to toss the oldest
		 * entry in the cache, but we hope that this
		 * condition doesn't happen very often.
		 */
		scp2 = scp;
		if (TAILQ_EMPTY(&scp2->sch_bucket)) {
			sce = &set->scs_buckethead[set->scs_size];
			for (++scp2; scp2 != scp; scp2++) {
				if (scp2 >= sce)
					scp2 = &set->scs_buckethead[0];
				if (! TAILQ_EMPTY(&scp2->sch_bucket))
					break;
			}
#ifdef DIAGNOSTIC
			/*
			 * This should never happen; we should always find a
			 * non-empty bucket.
			 */
			if (scp2 == scp)
				panic("%s: cacheoverflow: impossible",
				    __func__);
#endif
		}
		sc2 = TAILQ_FIRST(&scp2->sch_bucket);
		syn_cache_rm(sc2);
		syn_cache_put(sc2);
	}

	/*
	 * Initialize the entry's timer.
	 */
	sc->sc_rxttot = 0;
	sc->sc_rxtshift = 0;
	SYN_CACHE_TIMER_ARM(sc);

	/* Link it from tcpcb entry */
	LIST_INSERT_HEAD(&tp->t_sc, sc, sc_tpq);

	/* Put it into the bucket. */
	TAILQ_INSERT_TAIL(&scp->sch_bucket, sc, sc_bucketq);
	scp->sch_length++;
	sc->sc_set = set;
	set->scs_count++;
	set->scs_use--;

	tcpstat_inc(tcps_sc_added);

	/*
	 * If the active cache has exceeded its use limit and
	 * the passive syn cache is empty, exchange their roles.
	 */
	if (set->scs_use <= 0 &&
	    tcp_syn_cache[!tcp_syn_cache_active].scs_count == 0)
		tcp_syn_cache_active = !tcp_syn_cache_active;
}