void suspend_stratum(struct pool *pool)
{
	applog(LOG_INFO, "Closing socket for stratum pool %d", pool->pool_no);
	mutex_lock(&pool->stratum_lock);
	pool->stratum_active = false;
	pool->stratum_auth = false;
	curl_easy_cleanup(pool->stratum_curl);
	pool->stratum_curl = NULL;
	pool->sock = INVSOCK;
	mutex_unlock(&pool->stratum_lock);
}