static int proxy_to_virtual_server(REQUEST *request)
{
	REQUEST *fake;
	RAD_REQUEST_FUNP fun;

	if (!request->home_server || !request->home_server->server) return 0;

	if (request->parent) {
		RDEBUG2("WARNING: Cancelling proxy request to virtual server %s as this request was itself proxied.", request->home_server->server);
		return 0;
	}

	fake = request_alloc_fake(request);
	if (!fake) {
		RDEBUG2("WARNING: Out of memory");
		return 0;
	}

	fake->packet->vps = paircopy(request->proxy->vps);
	fake->server = request->home_server->server;

	if (request->proxy->code == PW_AUTHENTICATION_REQUEST) {
		fun = rad_authenticate;

#ifdef WITH_ACCOUNTING
	} else if (request->proxy->code == PW_ACCOUNTING_REQUEST) {
		fun = rad_accounting;
#endif

	} else {
		RDEBUG2("Unknown packet type %d", request->proxy->code);
		ev_request_free(&fake);
		return 0;
	}

	RDEBUG2(">>> Sending proxied request internally to virtual server.");
	radius_handle_request(fake, fun);
	RDEBUG2("<<< Received proxied response code %d from internal virtual server.", fake->reply->code);

	if (fake->reply->code != 0) {
		request->proxy_reply = fake->reply;
		fake->reply = NULL;
	} else {
		/*
		 *	There was no response
		 */
		setup_post_proxy_fail(request);
	}

	ev_request_free(&fake);

	process_proxy_reply(request);

	/*
	 *	Process it through the normal section again, but ONLY
	 *	if we received a proxy reply..
	 */
	if (request->proxy_reply) {
		if (request->server) RDEBUG("server %s {",
					    request->server != NULL ?
					    request->server : ""); 
		fun(request);
		
		if (request->server) RDEBUG("} # server %s",
					    request->server != NULL ?
					    request->server : "");
	}

	return 2;		/* success, but NOT '1' !*/
}