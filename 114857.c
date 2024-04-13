static bool parse_diff(struct pool *pool, json_t *val)
{
	double diff;

	diff = json_number_value(json_array_get(val, 0));
	if (diff == 0)
		return false;

	mutex_lock(&pool->pool_lock);
	pool->swork.diff = diff;
	mutex_unlock(&pool->pool_lock);

	applog(LOG_DEBUG, "Pool %d difficulty set to %f", pool->pool_no, diff);

	return true;
}