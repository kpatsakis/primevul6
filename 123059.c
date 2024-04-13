static int insert_into_proxy_hash(REQUEST *request, int retransmit)
{
	int i, proxy;
	char buf[128];

	rad_assert(request->proxy != NULL);
	rad_assert(proxy_list != NULL);

	PTHREAD_MUTEX_LOCK(&proxy_mutex);

	/*
	 *	Keep track of maximum outstanding requests to a
	 *	particular home server.  'max_outstanding' is
	 *	enforced in home_server_ldb(), in realms.c.
	 */
	if (request->home_server) {
		request->home_server->currently_outstanding++;
	}

	if (retransmit) {
		RADIUS_PACKET packet;

		packet = *request->proxy;

		if (!proxy_id_alloc(request, &packet)) {
			PTHREAD_MUTEX_UNLOCK(&proxy_mutex);
			return 0;
		}

		/*
		 *	Yank the request, free the old Id, and
		 *	remember the new Id.
		 */
		fr_packet_list_yank(proxy_list, request->proxy);
		fr_packet_list_id_free(proxy_list, request->proxy);
		*request->proxy = packet;

	} else if (!proxy_id_alloc(request, request->proxy)) {
		PTHREAD_MUTEX_UNLOCK(&proxy_mutex);
		return 0;
	}

	rad_assert(request->proxy->sockfd >= 0);

	/*
	 *	FIXME: Hack until we get rid of rad_listen_t, and put
	 *	the information into the packet_list.
	 */
	proxy = -1;
	for (i = 0; i < 32; i++) {
		if (proxy_fds[i] == request->proxy->sockfd) {
			proxy = i;
			break;
		}
	}

	if (proxy < 0) {
		PTHREAD_MUTEX_UNLOCK(&proxy_mutex);
		RDEBUG2("ERROR: All sockets are full.");
		return 0;
	}

	rad_assert(proxy_fds[proxy] != -1);
	rad_assert(proxy_listeners[proxy] != NULL);
	request->proxy_listener = proxy_listeners[proxy];

	if (!fr_packet_list_insert(proxy_list, &request->proxy)) {
		fr_packet_list_id_free(proxy_list, request->proxy);
		PTHREAD_MUTEX_UNLOCK(&proxy_mutex);
		RDEBUG2("ERROR: Failed to insert entry into proxy list");
		return 0;
	}

	request->in_proxy_hash = TRUE;

	PTHREAD_MUTEX_UNLOCK(&proxy_mutex);

	RDEBUG3(" proxy: allocating destination %s port %d - Id %d",
	       inet_ntop(request->proxy->dst_ipaddr.af,
			 &request->proxy->dst_ipaddr.ipaddr, buf, sizeof(buf)),
	       request->proxy->dst_port,
	       request->proxy->id);

	return 1;
}