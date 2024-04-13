static int request_pre_handler(REQUEST *request)
{
	int rcode;

	rad_assert(request->magic == REQUEST_MAGIC);
	rad_assert(request->packet != NULL);

	request->child_state = REQUEST_RUNNING;

	/*
	 *	Don't decode the packet if it's an internal "fake"
	 *	request.  Instead, just return so that the caller can
	 *	process it.
	 */
	if (request->packet->dst_port == 0) {
		request->username = pairfind(request->packet->vps,
					     PW_USER_NAME);
		request->password = pairfind(request->packet->vps,
					     PW_USER_PASSWORD);
		return 1;
	}

#ifdef WITH_PROXY
	/*
	 *	Put the decoded packet into it's proper place.
	 */
	if (request->proxy_reply != NULL) {
		rcode = request->proxy_listener->decode(request->proxy_listener,
							request);
		DEBUG_PACKET(request, request->proxy_reply, 0);
	} else
#endif
	if (request->packet->vps == NULL) {
		rcode = request->listener->decode(request->listener, request);
		
		if (debug_condition) {
			int result = FALSE;
			const char *my_debug = debug_condition;

			/*
			 *	Ignore parse errors.
			 */
			radius_evaluate_condition(request, RLM_MODULE_OK, 0,
						  &my_debug, 1,
						  &result);
			if (result) {
				request->options = 2;
				request->radlog = radlog_request;
			}
		}
		
		DEBUG_PACKET(request, request->packet, 0);
	} else {
		rcode = 0;
	}

	if (rcode < 0) {
		RDEBUG("%s Dropping packet without response.", fr_strerror());
		request->reply->offset = -2; /* bad authenticator */
		request->child_state = REQUEST_DONE;
		return 0;
	}

	if (!request->username) {
		request->username = pairfind(request->packet->vps,
					     PW_USER_NAME);
	}

#ifdef WITH_PROXY
	if (request->proxy) {
		return process_proxy_reply(request);
	}
#endif

	return 1;
}