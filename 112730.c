syn_cache_reaper(void *arg)
{
	struct syn_cache *sc = arg;

	pool_put(&syn_cache_pool, (sc));
	return;
}