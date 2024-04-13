void stratum_probe_transparency(struct pool *pool)
{
	// Request transaction data to discourage pools from doing anything shady
	char s[1024];
	int sLen;
	sLen = sprintf(s, "{\"params\": [\"%s\"], \"id\": \"txlist%s\", \"method\": \"mining.get_transactions\"}",
	        pool->swork.job_id,
	        pool->swork.job_id);
	stratum_send(pool, s, sLen);
	if ((!pool->swork.opaque) && pool->swork.transparency_time == (time_t)-1)
		pool->swork.transparency_time = time(NULL);
	pool->swork.transparency_probed = true;
}