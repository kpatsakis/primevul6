static bool parse_notify(struct pool *pool, json_t *val)
{
	char *job_id, *prev_hash, *coinbase1, *coinbase2, *bbversion, *nbit, *ntime;
	bool clean, ret = false;
	int merkles, i;
	json_t *arr;

	arr = json_array_get(val, 4);
	if (!arr || !json_is_array(arr))
		goto out;

	merkles = json_array_size(arr);

	job_id = json_array_string(val, 0);
	prev_hash = json_array_string(val, 1);
	coinbase1 = json_array_string(val, 2);
	coinbase2 = json_array_string(val, 3);
	bbversion = json_array_string(val, 5);
	nbit = json_array_string(val, 6);
	ntime = json_array_string(val, 7);
	clean = json_is_true(json_array_get(val, 8));

	if (!job_id || !prev_hash || !coinbase1 || !coinbase2 || !bbversion || !nbit || !ntime) {
		/* Annoying but we must not leak memory */
		if (job_id)
			free(job_id);
		if (prev_hash)
			free(prev_hash);
		if (coinbase1)
			free(coinbase1);
		if (coinbase2)
			free(coinbase2);
		if (bbversion)
			free(bbversion);
		if (nbit)
			free(nbit);
		if (ntime)
			free(ntime);
		goto out;
	}

	mutex_lock(&pool->pool_lock);
	free(pool->swork.job_id);
	free(pool->swork.prev_hash);
	free(pool->swork.coinbase1);
	free(pool->swork.coinbase2);
	free(pool->swork.bbversion);
	free(pool->swork.nbit);
	free(pool->swork.ntime);
	pool->swork.job_id = job_id;
	pool->swork.prev_hash = prev_hash;
	pool->swork.coinbase1 = coinbase1;
	pool->swork.cb1_len = strlen(coinbase1) / 2;
	pool->swork.coinbase2 = coinbase2;
	pool->swork.cb2_len = strlen(coinbase2) / 2;
	pool->swork.bbversion = bbversion;
	pool->swork.nbit = nbit;
	pool->swork.ntime = ntime;
	pool->submit_old = !clean;
	pool->swork.clean = true;
	pool->swork.cb_len = pool->swork.cb1_len + pool->n1_len + pool->n2size + pool->swork.cb2_len;

	for (i = 0; i < pool->swork.merkles; i++)
		free(pool->swork.merkle[i]);
	if (merkles) {
		pool->swork.merkle = realloc(pool->swork.merkle, sizeof(char *) * merkles + 1);
		for (i = 0; i < merkles; i++)
			pool->swork.merkle[i] = json_array_string(arr, i);
	}
	pool->swork.merkles = merkles;
	if (clean)
		pool->nonce2 = 0;
	pool->swork.header_len = strlen(pool->swork.bbversion) +
				 strlen(pool->swork.prev_hash) +
				 strlen(pool->swork.ntime) +
				 strlen(pool->swork.nbit) +
	/* merkle_hash */	 32 +
	/* nonce */		 8 +
	/* workpadding */	 96;
	pool->swork.header_len = pool->swork.header_len * 2 + 1;
	align_len(&pool->swork.header_len);
	mutex_unlock(&pool->pool_lock);

	applog(LOG_DEBUG, "Received stratum notify from pool %u with job_id=%s",
	       pool->pool_no, job_id);
	if (opt_protocol) {
		applog(LOG_DEBUG, "job_id: %s", job_id);
		applog(LOG_DEBUG, "prev_hash: %s", prev_hash);
		applog(LOG_DEBUG, "coinbase1: %s", coinbase1);
		applog(LOG_DEBUG, "coinbase2: %s", coinbase2);
		for (i = 0; i < merkles; i++)
			applog(LOG_DEBUG, "merkle%d: %s", i, pool->swork.merkle[i]);
		applog(LOG_DEBUG, "bbversion: %s", bbversion);
		applog(LOG_DEBUG, "nbit: %s", nbit);
		applog(LOG_DEBUG, "ntime: %s", ntime);
		applog(LOG_DEBUG, "clean: %s", clean ? "yes" : "no");
	}

	/* A notify message is the closest stratum gets to a getwork */
	pool->getwork_requested++;
	total_getworks++;

	if ((merkles && (!pool->swork.transparency_probed || rand() <= RAND_MAX / (opt_skip_checks + 1))) || pool->swork.transparency_time != (time_t)-1)
		if (pool->stratum_auth)
			stratum_probe_transparency(pool);

	ret = true;
out:
	return ret;
}