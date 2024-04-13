static REQUEST *lookup_in_proxy_hash(RADIUS_PACKET *reply)
{
	RADIUS_PACKET **proxy_p;
	REQUEST *request;

	PTHREAD_MUTEX_LOCK(&proxy_mutex);
	proxy_p = fr_packet_list_find_byreply(proxy_list, reply);

	if (!proxy_p) {
		PTHREAD_MUTEX_UNLOCK(&proxy_mutex);
		return NULL;
	}

	request = fr_packet2myptr(REQUEST, proxy, proxy_p);

	if (!request) {
		PTHREAD_MUTEX_UNLOCK(&proxy_mutex);
		return NULL;
	}

	request->num_proxied_responses++;

	/*
	 *	Catch the most common case of everything working
	 *	correctly.
	 */
	if (request->num_proxied_requests == request->num_proxied_responses) {
		fr_packet_list_yank(proxy_list, request->proxy);
		fr_packet_list_id_free(proxy_list, request->proxy);
		request->in_proxy_hash = FALSE;
	}

	/*
	 *	On the FIRST reply, decrement the count of outstanding
	 *	requests.  Note that this is NOT the count of sent
	 *	packets, but whether or not the home server has
	 *	responded at all.
	 */
	if (!request->proxy_reply &&
	    request->home_server &&
	    request->home_server->currently_outstanding) {
		request->home_server->currently_outstanding--;
	}

	PTHREAD_MUTEX_UNLOCK(&proxy_mutex);

	return request;
}