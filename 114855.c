static void recalloc_sock(struct pool *pool, size_t len)
{
	size_t old, new;

	old = strlen(pool->sockbuf);
	new = old + len + 1;
	if (new < pool->sockbuf_size)
		return;
	new = new + (RBUFSIZE - (new % RBUFSIZE));
	applog(LOG_DEBUG, "Recallocing pool sockbuf to %lu", (unsigned long)new);
	pool->sockbuf = realloc(pool->sockbuf, new);
	if (!pool->sockbuf)
		quit(1, "Failed to realloc pool sockbuf in recalloc_sock");
	memset(pool->sockbuf + old, 0, new - old);
	pool->sockbuf_size = new;
}