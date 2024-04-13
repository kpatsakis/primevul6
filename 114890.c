static void clear_sock(struct pool *pool)
{
	ssize_t n;

	mutex_lock(&pool->stratum_lock);
	do
		n = recv(pool->sock, pool->sockbuf, RECVSIZE, 0);
	while (n > 0);
	mutex_unlock(&pool->stratum_lock);
	strcpy(pool->sockbuf, "");
}