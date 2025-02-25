static bool http_negotiate(struct pool *pool, int sockd, bool http0)
{
	char buf[1024];
	int i, len;

	if (http0) {
		snprintf(buf, 1024, "CONNECT %s:%s HTTP/1.0\r\n\r\n",
			pool->sockaddr_url, pool->stratum_port);
	} else {
		snprintf(buf, 1024, "CONNECT %s:%s HTTP/1.1\r\nHost: %s:%s\r\n\r\n",
			pool->sockaddr_url, pool->stratum_port, pool->sockaddr_url,
			pool->stratum_port);
	}
	applog(LOG_DEBUG, "Sending proxy %s:%s - %s",
		pool->sockaddr_proxy_url, pool->sockaddr_proxy_port, buf);
	send(sockd, buf, strlen(buf), 0);
	len = recv(sockd, buf, 12, 0);
	if (len <= 0) {
		applog(LOG_WARNING, "Couldn't read from proxy %s:%s after sending CONNECT",
		       pool->sockaddr_proxy_url, pool->sockaddr_proxy_port);
		return false;
	}
	buf[len] = '\0';
	applog(LOG_DEBUG, "Received from proxy %s:%s - %s",
	       pool->sockaddr_proxy_url, pool->sockaddr_proxy_port, buf);
	if (strcmp(buf, "HTTP/1.1 200") && strcmp(buf, "HTTP/1.0 200")) {
		applog(LOG_WARNING, "HTTP Error from proxy %s:%s - %s",
		       pool->sockaddr_proxy_url, pool->sockaddr_proxy_port, buf);
		return false;
	}

	/* Ignore unwanted headers till we get desired response */
	for (i = 0; i < 4; i++) {
		buf[i] = recv_byte(sockd);
		if (buf[i] == (char)-1) {
			applog(LOG_WARNING, "Couldn't read HTTP byte from proxy %s:%s",
			pool->sockaddr_proxy_url, pool->sockaddr_proxy_port);
			return false;
		}
	}
	while (strncmp(buf, "\r\n\r\n", 4)) {
		for (i = 0; i < 3; i++)
			buf[i] = buf[i + 1];
		buf[3] = recv_byte(sockd);
		if (buf[3] == (char)-1) {
			applog(LOG_WARNING, "Couldn't read HTTP byte from proxy %s:%s",
			pool->sockaddr_proxy_url, pool->sockaddr_proxy_port);
			return false;
		}
	}

	applog(LOG_DEBUG, "Success negotiating with %s:%s HTTP proxy",
	       pool->sockaddr_proxy_url, pool->sockaddr_proxy_port);
	return true;
}