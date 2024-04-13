bool stratum_send(struct pool *pool, char *s, ssize_t len)
{
	bool ret = false;

	mutex_lock(&pool->stratum_lock);
	if (pool->stratum_active)
		ret = __stratum_send(pool, s, len);
	else
		applog(LOG_DEBUG, "Stratum send failed due to no pool stratum_active");
	mutex_unlock(&pool->stratum_lock);

	return ret;
}