static int proxy_id_alloc(REQUEST *request, RADIUS_PACKET *packet)
{
	int i, proxy, found;
	rad_listen_t *proxy_listener;

	if (fr_packet_list_id_alloc(proxy_list, packet)) return 1;

	if (proxy_all_used) return 0;

	/*
	 *	Allocate a new proxy fd.  This function adds
	 *	it to the tail of the list of listeners.  With
	 *	some care, this can be thread-safe.
	 */
	proxy_listener = proxy_new_listener(&packet->src_ipaddr, FALSE);
	if (!proxy_listener) {
		radlog(L_PROXY, "Failed to create a new socket for proxying requests.");
		return 0;
	}
	
	/*
	 *	Cache it locally.
	 */
	found = -1;
	proxy = proxy_listener->fd;
	for (i = 0; i < 32; i++) {
		/*
		 *	Found a free entry.  Save the socket,
		 *	and remember where we saved it.
		 */
		if (proxy_fds[(proxy + i) & 0x1f] == -1) {
			found = (proxy + i) & 0x1f;
			proxy_fds[found] = proxy;
			proxy_listeners[found] = proxy_listener;
			break;
		}
	}
	if (found < 0) {
		proxy_all_used = TRUE;
		listen_free(&proxy_listener);
		radlog(L_ERR, "Failed creating new proxy socket: server is too busy and home servers appear to be down");
		return 0;
	}

	
	if (!fr_packet_list_socket_add(proxy_list, proxy_listener->fd)) {
			RDEBUG2("ERROR: Failed to create a new socket for proxying requests.");
		return 0;
		
	}
	
	if (!fr_packet_list_id_alloc(proxy_list, packet)) {
			RDEBUG2("ERROR: Failed to create a new socket for proxying requests.");
		return 0;
	}
	
	/*
	 *	Signal the main thread to add the new FD to the list
	 *	of listening FD's.
	 */
	radius_signal_self(RADIUS_SIGNAL_SELF_NEW_FD);
	return 1;
}