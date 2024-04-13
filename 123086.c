static int proxy_request(REQUEST *request)
{
	struct timeval when;
	char buffer[128];

#ifdef WITH_COA
	if (request->coa) {
		RDEBUG("WARNING: Cannot proxy and originate CoA packets at the same time.  Cancelling CoA request");
		ev_request_free(&request->coa);
	}
#endif

	if (request->home_server->server) {
		RDEBUG("ERROR: Cannot perform real proxying to a virtual server.");
		return 0;
	}

	if (!insert_into_proxy_hash(request, FALSE)) {
		RDEBUG("ERROR: Failed inserting request into proxy hash.");
		return 0;
	}

	request->proxy_listener->encode(request->proxy_listener, request);

	when = request->received;
	when.tv_sec += request->root->max_request_time;

	gettimeofday(&request->proxy_when, NULL);

	request->next_when = request->proxy_when;
	request->next_when.tv_sec += request->home_server->response_window;

	rad_assert(request->home_server->response_window > 0);

	if (timercmp(&when, &request->next_when, <)) {
		request->next_when = when;
	}
	request->next_callback = no_response_to_proxied_request;

	RDEBUG2("Proxying request %u to home server %s port %d",
	       request->number,
	       inet_ntop(request->proxy->dst_ipaddr.af,
			 &request->proxy->dst_ipaddr.ipaddr,
			 buffer, sizeof(buffer)),
	       request->proxy->dst_port);

	/*
	 *	Note that we set proxied BEFORE sending the packet.
	 *
	 *	Once we send it, the request is tainted, as
	 *	another thread may have picked it up.  Don't
	 *	touch it!
	 */
	request->num_proxied_requests = 1;
	request->num_proxied_responses = 0;
#ifdef HAVE_PTHREAD_H
	request->child_pid = NO_SUCH_CHILD_PID;
#endif
	request->child_state = REQUEST_PROXIED;

	DEBUG_PACKET(request, request->proxy, 1);

	request->proxy_listener->send(request->proxy_listener,
				      request);
	return 1;
}