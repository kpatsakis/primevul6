static bool setup_stratum_curl(struct pool *pool)
{
	char curl_err_str[CURL_ERROR_SIZE];
	CURL *curl = NULL;
	char s[RBUFSIZE];

	applog(LOG_DEBUG, "initiate_stratum with sockbuf=%p", pool->sockbuf);
	mutex_lock(&pool->stratum_lock);
	pool->swork.transparency_time = (time_t)-1;
	pool->stratum_active = false;
	pool->stratum_auth = false;
	pool->stratum_notify = false;
	pool->swork.transparency_probed = false;
	if (!pool->stratum_curl) {
		pool->stratum_curl = curl_easy_init();
		if (unlikely(!pool->stratum_curl))
			quit(1, "Failed to curl_easy_init in initiate_stratum");
	}
	if (pool->sockbuf)
		pool->sockbuf[0] = '\0';
	mutex_unlock(&pool->stratum_lock);
	curl = pool->stratum_curl;

	if (!pool->sockbuf) {
		pool->sockbuf = calloc(RBUFSIZE, 1);
		if (!pool->sockbuf)
			quit(1, "Failed to calloc pool sockbuf in initiate_stratum");
		pool->sockbuf_size = RBUFSIZE;
	}

	/* Create a http url for use with curl */
	sprintf(s, "http://%s:%s", pool->sockaddr_url, pool->stratum_port);

	curl_easy_setopt(curl, CURLOPT_FRESH_CONNECT, 1);
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 30);
	curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, curl_err_str);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	curl_easy_setopt(curl, CURLOPT_URL, s);
	curl_easy_setopt(curl, CURLOPT_TCP_NODELAY, 1);

	/* We use DEBUGFUNCTION to count bytes sent/received, and verbose is needed
	 * to enable it */
	curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, curl_debug_cb);
	curl_easy_setopt(curl, CURLOPT_DEBUGDATA, (void *)pool);
	curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);

	// CURLINFO_LASTSOCKET is broken on Win64 (which has a wider SOCKET type than curl_easy_getinfo returns), so we use this hack for now
	curl_easy_setopt(curl, CURLOPT_OPENSOCKETFUNCTION, grab_socket_opensocket_cb);
	curl_easy_setopt(curl, CURLOPT_OPENSOCKETDATA, pool);
	
	curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_TRY);
	if (pool->rpc_proxy) {
		curl_easy_setopt(curl, CURLOPT_PROXY, pool->rpc_proxy);
	} else if (opt_socks_proxy) {
		curl_easy_setopt(curl, CURLOPT_PROXY, opt_socks_proxy);
		curl_easy_setopt(curl, CURLOPT_PROXYTYPE, CURLPROXY_SOCKS4);
	}
	curl_easy_setopt(curl, CURLOPT_CONNECT_ONLY, 1);
	pool->sock = INVSOCK;
	if (curl_easy_perform(curl)) {
		applog(LOG_INFO, "Stratum connect failed to pool %d: %s", pool->pool_no, curl_err_str);
		return false;
	}
	if (pool->sock == INVSOCK)
	{
		curl_easy_cleanup(curl);
		applog(LOG_ERR, "Stratum connect succeeded, but technical problem extracting socket (pool %u)", pool->pool_no);
		return false;
	}
	keep_sockalive(pool->sock);

	pool->cgminer_pool_stats.times_sent++;
	pool->cgminer_pool_stats.times_received++;

	return true;
}