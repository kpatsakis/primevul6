json_t *json_rpc_call_completed(CURL *curl, int rc, bool probe, int *rolltime, void *out_priv)
{
	struct json_rpc_call_state *state;
	if (curl_easy_getinfo(curl, CURLINFO_PRIVATE, &state) != CURLE_OK) {
		applog(LOG_ERR, "Failed to get private curl data");
		if (out_priv)
			*(void**)out_priv = NULL;
		goto err_out;
	}
	if (out_priv)
		*(void**)out_priv = state->priv;

	json_t *val, *err_val, *res_val;
	json_error_t err;
	struct pool *pool = state->pool;
	bool probing = probe && !pool->probed;

	if (rc) {
		applog(LOG_INFO, "HTTP request failed: %s", state->curl_err_str);
		goto err_out;
	}

	if (!state->all_data.buf) {
		applog(LOG_DEBUG, "Empty data received in json_rpc_call.");
		goto err_out;
	}

	pool->cgminer_pool_stats.times_sent++;
	pool->cgminer_pool_stats.times_received++;

	if (probing) {
		pool->probed = true;
		/* If X-Long-Polling was found, activate long polling */
		if (state->hi.lp_path) {
			if (pool->hdr_path != NULL)
				free(pool->hdr_path);
			pool->hdr_path = state->hi.lp_path;
		} else
			pool->hdr_path = NULL;
		if (state->hi.stratum_url) {
			pool->stratum_url = state->hi.stratum_url;
			state->hi.stratum_url = NULL;
		}
	} else {
		if (state->hi.lp_path) {
			free(state->hi.lp_path);
			state->hi.lp_path = NULL;
		}
		if (state->hi.stratum_url) {
			free(state->hi.stratum_url);
			state->hi.stratum_url = NULL;
		}
	}

	if (pool->force_rollntime)
	{
		state->hi.canroll = true;
		state->hi.hadexpire = true;
		state->hi.rolltime = pool->force_rollntime;
	}
	
	if (rolltime)
		*rolltime = state->hi.rolltime;
	pool->cgminer_pool_stats.rolltime = state->hi.rolltime;
	pool->cgminer_pool_stats.hadrolltime = state->hi.hadrolltime;
	pool->cgminer_pool_stats.canroll = state->hi.canroll;
	pool->cgminer_pool_stats.hadexpire = state->hi.hadexpire;

	val = JSON_LOADS(state->all_data.buf, &err);
	if (!val) {
		applog(LOG_INFO, "JSON decode failed(%d): %s", err.line, err.text);

		if (opt_protocol)
			applog(LOG_DEBUG, "JSON protocol response:\n%s", (char*)state->all_data.buf);

		goto err_out;
	}

	if (opt_protocol) {
		char *s = json_dumps(val, JSON_INDENT(3));

		applog(LOG_DEBUG, "JSON protocol response:\n%s", s);
		free(s);
	}

	/* JSON-RPC valid response returns a non-null 'result',
	 * and a null 'error'.
	 */
	res_val = json_object_get(val, "result");
	err_val = json_object_get(val, "error");

	if (!res_val ||(err_val && !json_is_null(err_val))) {
		char *s;

		if (err_val)
			s = json_dumps(err_val, JSON_INDENT(3));
		else
			s = strdup("(unknown reason)");

		applog(LOG_INFO, "JSON-RPC call failed: %s", s);

		free(s);
		json_decref(val);

		goto err_out;
	}

	if (state->hi.reason) {
		json_object_set_new(val, "reject-reason", json_string(state->hi.reason));
		free(state->hi.reason);
		state->hi.reason = NULL;
	}
	successful_connect = true;
	databuf_free(&state->all_data);
	curl_slist_free_all(state->headers);
	curl_easy_reset(curl);
	free(state);
	return val;

err_out:
	databuf_free(&state->all_data);
	curl_slist_free_all(state->headers);
	curl_easy_reset(curl);
	if (!successful_connect)
		applog(LOG_DEBUG, "Failed to connect in json_rpc_call");
	curl_easy_setopt(curl, CURLOPT_FRESH_CONNECT, 1);
	free(state);
	return NULL;
}