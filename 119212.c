static enum send_ret __stratum_send(struct pool *pool, char *s, ssize_t len)
{
	SOCKETTYPE sock = pool->sock;
	ssize_t ssent = 0;

	strcat(s, "\n");
	len++;

	while (len > 0 ) {
		struct timeval timeout = {1, 0};
		ssize_t sent;
		fd_set wd;
retry:
		FD_ZERO(&wd);
		FD_SET(sock, &wd);
		if (select(sock + 1, NULL, &wd, NULL, &timeout) < 1) {
			if (interrupted())
				goto retry;
			return SEND_SELECTFAIL;
		}
#ifdef __APPLE__
		sent = send(pool->sock, s + ssent, len, SO_NOSIGPIPE);
#elif WIN32
		sent = send(pool->sock, s + ssent, len, 0);
#else
		sent = send(pool->sock, s + ssent, len, MSG_NOSIGNAL);
#endif
		if (sent < 0) {
			if (!sock_blocks())
				return SEND_SENDFAIL;
			sent = 0;
		}
		ssent += sent;
		len -= sent;
	}

	pool->sgminer_pool_stats.times_sent++;
	pool->sgminer_pool_stats.bytes_sent += ssent;
	pool->sgminer_pool_stats.net_bytes_sent += ssent;
	return SEND_OK;
}