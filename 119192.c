bool stratum_send(struct pool *pool, char *s, ssize_t len)
{
	enum send_ret ret = SEND_INACTIVE;

	if (opt_protocol)
		applog(LOG_DEBUG, "SEND: %s", s);

	mutex_lock(&pool->stratum_lock);
	if (pool->stratum_active)
		ret = __stratum_send(pool, s, len);
	mutex_unlock(&pool->stratum_lock);

	/* This is to avoid doing applog under stratum_lock */
	switch (ret) {
		default:
		case SEND_OK:
			break;
		case SEND_SELECTFAIL:
			applog(LOG_DEBUG, "Write select failed on %s sock", get_pool_name(pool));
			suspend_stratum(pool);
			break;
		case SEND_SENDFAIL:
			applog(LOG_DEBUG, "Failed to send in stratum_send");
			suspend_stratum(pool);
			break;
		case SEND_INACTIVE:
			applog(LOG_DEBUG, "Stratum send failed due to no pool stratum_active");
			break;
	}
	return (ret == SEND_OK);
}