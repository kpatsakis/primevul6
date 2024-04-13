void radius_event_free(void)
{
	/*
	 *	FIXME: Stop all threads, or at least check that
	 *	they're all waiting on the semaphore, and the queues
	 *	are empty.
	 */

#ifdef WITH_PROXY
	/*
	 *	There are requests in the proxy hash that aren't
	 *	referenced from anywhere else.  Remove them first.
	 */
	if (proxy_list) {
		fr_packet_list_walk(proxy_list, NULL, proxy_hash_cb);
		fr_packet_list_free(proxy_list);
		proxy_list = NULL;
	}
#endif

	fr_packet_list_walk(pl, NULL, request_hash_cb);

	fr_packet_list_free(pl);
	pl = NULL;

	fr_event_list_free(el);
}