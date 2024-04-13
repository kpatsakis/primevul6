static bool socket_full(struct pool *pool, bool wait)
{
	SOCKETTYPE sock = pool->sock;
	struct timeval timeout;
	fd_set rd;

	FD_ZERO(&rd);
	FD_SET(sock, &rd);
	timeout.tv_usec = 0;
	if (wait)
		timeout.tv_sec = 60;
	else
		timeout.tv_sec = 0;
	if (select(sock + 1, &rd, NULL, NULL, &timeout) > 0)
		return true;
	return false;
}