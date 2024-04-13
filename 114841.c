void json_rpc_call_async(CURL *curl, const char *url,
		      const char *userpass, const char *rpc_req,
		      bool longpoll,
		      struct pool *pool, bool share,
		      void *priv)
{
	struct json_rpc_call_state *state = malloc(sizeof(struct json_rpc_call_state));
	*state = (struct json_rpc_call_state){
		.priv = priv,
		.pool = pool,
	};
	long timeout = longpoll ? (60 * 60) : 60;
	char len_hdr[64], user_agent_hdr[128];
	struct curl_slist *headers = NULL;

	if (longpoll)
		state->all_data.idlemarker = &pool->lp_socket;

	/* it is assumed that 'curl' is freshly [re]initialized at this pt */

	curl_easy_setopt(curl, CURLOPT_PRIVATE, state);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout);

	/* We use DEBUGFUNCTION to count bytes sent/received, and verbose is needed
	 * to enable it */
	curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, curl_debug_cb);
	curl_easy_setopt(curl, CURLOPT_DEBUGDATA, (void *)pool);
	curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);

	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_setopt(curl, CURLOPT_ENCODING, "");
	curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1);

	/* Shares are staggered already and delays in submission can be costly
	 * so do not delay them */
	if (!opt_delaynet || share)
		curl_easy_setopt(curl, CURLOPT_TCP_NODELAY, 1);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, all_data_cb);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &state->all_data);
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, upload_data_cb);
	curl_easy_setopt(curl, CURLOPT_READDATA, &state->upload_data);
	curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, &state->curl_err_str[0]);
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, resp_hdr_cb);
	curl_easy_setopt(curl, CURLOPT_HEADERDATA, &state->hi);

	curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_TRY);
	if (pool->rpc_proxy) {
		curl_easy_setopt(curl, CURLOPT_PROXY, pool->rpc_proxy);
	} else if (opt_socks_proxy) {
		curl_easy_setopt(curl, CURLOPT_PROXY, opt_socks_proxy);
		curl_easy_setopt(curl, CURLOPT_PROXYTYPE, CURLPROXY_SOCKS4);
	}
	if (userpass) {
		curl_easy_setopt(curl, CURLOPT_USERPWD, userpass);
		curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
	}
	if (longpoll)
		curl_easy_setopt(curl, CURLOPT_SOCKOPTFUNCTION, json_rpc_call_sockopt_cb);
	curl_easy_setopt(curl, CURLOPT_POST, 1);

	if (opt_protocol)
		applog(LOG_DEBUG, "JSON protocol request:\n%s", rpc_req);

	state->upload_data.buf = rpc_req;
	state->upload_data.len = strlen(rpc_req);
	sprintf(len_hdr, "Content-Length: %lu",
		(unsigned long) state->upload_data.len);
	sprintf(user_agent_hdr, "User-Agent: %s", PACKAGE_STRING);

	headers = curl_slist_append(headers,
		"Content-type: application/json");
	headers = curl_slist_append(headers,
		"X-Mining-Extensions: longpoll midstate rollntime submitold");

	if (longpoll)
		headers = curl_slist_append(headers,
			"X-Minimum-Wait: 0");

	if (likely(global_hashrate)) {
		char ghashrate[255];

		sprintf(ghashrate, "X-Mining-Hashrate: %"PRIu64, (uint64_t)global_hashrate);
		headers = curl_slist_append(headers, ghashrate);
	}

	headers = curl_slist_append(headers, len_hdr);
	headers = curl_slist_append(headers, user_agent_hdr);
	headers = curl_slist_append(headers, "Expect:"); /* disable Expect hdr*/

	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
	state->headers = headers;

	if (opt_delaynet) {
		/* Don't delay share submission, but still track the nettime */
		if (!share) {
			long long now_msecs, last_msecs;
			struct timeval now, last;

			gettimeofday(&now, NULL);
			last_nettime(&last);
			now_msecs = (long long)now.tv_sec * 1000;
			now_msecs += now.tv_usec / 1000;
			last_msecs = (long long)last.tv_sec * 1000;
			last_msecs += last.tv_usec / 1000;
			if (now_msecs > last_msecs && now_msecs - last_msecs < 250) {
				struct timespec rgtp;

				rgtp.tv_sec = 0;
				rgtp.tv_nsec = (250 - (now_msecs - last_msecs)) * 1000000;
				nanosleep(&rgtp, NULL);
			}
		}
		set_nettime();
	}
}