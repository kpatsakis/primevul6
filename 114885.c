static bool __stratum_send(struct pool *pool, char *s, ssize_t len)
{
	SOCKETTYPE sock = pool->sock;
	ssize_t ssent = 0;

	if (opt_protocol)
		applog(LOG_DEBUG, "SEND: %s", s);

	strcat(s, "\n");
	len++;

	while (len > 0 ) {
		struct timeval timeout = {0, 0};
		ssize_t sent;
		fd_set wd;

		FD_ZERO(&wd);
		FD_SET(sock, &wd);
		if (select(sock + 1, NULL, &wd, NULL, &timeout) < 1) {
			applog(LOG_DEBUG, "Write select failed on pool %d sock", pool->pool_no);
			return false;
		}
		sent = send(pool->sock, s + ssent, len, 0);
		if (sent < 0) {
			if (errno != EAGAIN && errno != EWOULDBLOCK) {
				applog(LOG_DEBUG, "Failed to curl_easy_send in stratum_send");
				return false;
			}
			sent = 0;
		}
		ssent += sent;
		len -= sent;
	}

	pool->cgminer_pool_stats.times_sent++;
	pool->cgminer_pool_stats.bytes_sent += ssent;
	total_bytes_xfer += ssent;
	pool->cgminer_pool_stats.net_bytes_sent += ssent;
	return true;
}