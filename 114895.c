json_t *json_rpc_call(CURL *curl, const char *url,
		      const char *userpass, const char *rpc_req,
		      bool probe, bool longpoll, int *rolltime,
		      struct pool *pool, bool share)
{
	json_rpc_call_async(curl, url, userpass, rpc_req, longpoll, pool, share, NULL);
	int rc = curl_easy_perform(curl);
	return json_rpc_call_completed(curl, rc, probe, rolltime, NULL);
}