void suspend_stratum(struct pool *pool)
{
	applog(LOG_INFO, "Closing socket for stratum %s", get_pool_name(pool));

	mutex_lock(&pool->stratum_lock);
	__suspend_stratum(pool);
	mutex_unlock(&pool->stratum_lock);
}