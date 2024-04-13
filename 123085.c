static int process_proxy_reply(REQUEST *request)
{
	int rcode;
	int post_proxy_type = 0;
	VALUE_PAIR *vp;
	
	/*
	 *	Delete any reply we had accumulated until now.
	 */
	pairfree(&request->reply->vps);
	
	/*
	 *	Run the packet through the post-proxy stage,
	 *	BEFORE playing games with the attributes.
	 */
	vp = pairfind(request->config_items, PW_POST_PROXY_TYPE);
	if (vp) {
		RDEBUG2("  Found Post-Proxy-Type %s", vp->vp_strvalue);
		post_proxy_type = vp->vp_integer;
	}
	
	if (request->home_pool && request->home_pool->virtual_server) {
		const char *old_server = request->server;
		
		request->server = request->home_pool->virtual_server;
		RDEBUG2(" server %s {", request->server);
		rcode = module_post_proxy(post_proxy_type, request);
		RDEBUG2(" }");
		request->server = old_server;
	} else {
		rcode = module_post_proxy(post_proxy_type, request);
	}

#ifdef WITH_COA
	if (request->packet->code == request->proxy->code)
	  /*
	   *	Don't run the next bit if we originated a CoA
	   *	packet, after receiving an Access-Request or
	   *	Accounting-Request.
	   */
#endif
	
	/*
	 *	There may NOT be a proxy reply, as we may be
	 *	running Post-Proxy-Type = Fail.
	 */
	if (request->proxy_reply) {
		/*
		 *	Delete the Proxy-State Attributes from
		 *	the reply.  These include Proxy-State
		 *	attributes from us and remote server.
		 */
		pairdelete(&request->proxy_reply->vps, PW_PROXY_STATE);
		
		/*
		 *	Add the attributes left in the proxy
		 *	reply to the reply list.
		 */
		pairadd(&request->reply->vps, request->proxy_reply->vps);
		request->proxy_reply->vps = NULL;
		
		/*
		 *	Free proxy request pairs.
		 */
		pairfree(&request->proxy->vps);
	}
	
	switch (rcode) {
	default:  /* Don't do anything */
		break;
	case RLM_MODULE_FAIL:
		/* FIXME: debug print stuff */
		request->child_state = REQUEST_DONE;
		return 0;
		
	case RLM_MODULE_HANDLED:
		/* FIXME: debug print stuff */
		request->child_state = REQUEST_DONE;
		return 0;
	}

	return 1;
}