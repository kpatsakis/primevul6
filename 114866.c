char *recv_line(struct pool *pool)
{
	ssize_t len, buflen;
	char *tok, *sret = NULL;

	if (!strstr(pool->sockbuf, "\n")) {
		struct timeval rstart, now;

		gettimeofday(&rstart, NULL);
		if (!socket_full(pool, true)) {
			applog(LOG_DEBUG, "Timed out waiting for data on socket_full");
			goto out;
		}

		mutex_lock(&pool->stratum_lock);
		do {
			char s[RBUFSIZE];
			size_t slen, n;

			memset(s, 0, RBUFSIZE);
			n = recv(pool->sock, s, RECVSIZE, 0);
			if (n < 1 && errno != EAGAIN && errno != EWOULDBLOCK) {
				applog(LOG_DEBUG, "Failed to recv sock in recv_line");
				break;
			}
			slen = strlen(s);
			recalloc_sock(pool, slen);
			strcat(pool->sockbuf, s);
			gettimeofday(&now, NULL);
		} while (tdiff(&now, &rstart) < 60 && !strstr(pool->sockbuf, "\n"));
		mutex_unlock(&pool->stratum_lock);
	}

	buflen = strlen(pool->sockbuf);
	tok = strtok(pool->sockbuf, "\n");
	if (!tok) {
		applog(LOG_DEBUG, "Failed to parse a \\n terminated string in recv_line");
		goto out;
	}
	sret = strdup(tok);
	len = strlen(sret);

	/* Copy what's left in the buffer after the \n, including the
	 * terminating \0 */
	if (buflen > len + 1)
		memmove(pool->sockbuf, pool->sockbuf + len + 1, buflen - len + 1);
	else
		strcpy(pool->sockbuf, "");

	pool->cgminer_pool_stats.times_received++;
	pool->cgminer_pool_stats.bytes_received += len;
	total_bytes_xfer += len;
	pool->cgminer_pool_stats.net_bytes_received += len;

out:
	if (!sret)
		clear_sock(pool);
	else if (opt_protocol)
		applog(LOG_DEBUG, "RECVD: %s", sret);
	return sret;
}