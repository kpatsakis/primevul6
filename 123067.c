static void remove_from_proxy_hash(REQUEST *request)
{
	/*
	 *	Check this without grabbing the mutex because it's a
	 *	lot faster that way.
	 */
	if (!request->in_proxy_hash) return;

	/*
	 *	The "not in hash" flag is definitive.  However, if the
	 *	flag says that it IS in the hash, there might still be
	 *	a race condition where it isn't.
	 */
	PTHREAD_MUTEX_LOCK(&proxy_mutex);

	if (!request->in_proxy_hash) {
		PTHREAD_MUTEX_UNLOCK(&proxy_mutex);
		return;
	}

	fr_packet_list_yank(proxy_list, request->proxy);
	fr_packet_list_id_free(proxy_list, request->proxy);

	/*
	 *	The home server hasn't replied, but we've given up on
	 *	this request.  Don't count this request against the
	 *	home server.
	 */
	if (!request->proxy_reply &&
	    request->home_server &&
	    request->home_server->currently_outstanding) {
		request->home_server->currently_outstanding--;
	}

	/*
	 *	Got from YES in hash, to NO, not in hash while we hold
	 *	the mutex.  This guarantees that when another thread
	 *	grans the mutex, the "not in hash" flag is correct.
	 */
	request->in_proxy_hash = FALSE;

  	PTHREAD_MUTEX_UNLOCK(&proxy_mutex);
}