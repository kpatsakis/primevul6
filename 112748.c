syn_cache_init(void)
{
	int i;

	/* Initialize the hash buckets. */
	tcp_syn_cache[0].scs_buckethead = mallocarray(tcp_syn_hash_size,
	    sizeof(struct syn_cache_head), M_SYNCACHE, M_WAITOK|M_ZERO);
	tcp_syn_cache[1].scs_buckethead = mallocarray(tcp_syn_hash_size,
	    sizeof(struct syn_cache_head), M_SYNCACHE, M_WAITOK|M_ZERO);
	tcp_syn_cache[0].scs_size = tcp_syn_hash_size;
	tcp_syn_cache[1].scs_size = tcp_syn_hash_size;
	for (i = 0; i < tcp_syn_hash_size; i++) {
		TAILQ_INIT(&tcp_syn_cache[0].scs_buckethead[i].sch_bucket);
		TAILQ_INIT(&tcp_syn_cache[1].scs_buckethead[i].sch_bucket);
	}

	/* Initialize the syn cache pool. */
	pool_init(&syn_cache_pool, sizeof(struct syn_cache), 0, IPL_SOFTNET,
	    0, "syncache", NULL);
}