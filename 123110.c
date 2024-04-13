static int successfully_proxied_request(REQUEST *request)
{
	int rcode;
	int pre_proxy_type = 0;
	VALUE_PAIR *realmpair;
	VALUE_PAIR *strippedname;
	VALUE_PAIR *vp;
	char *realmname = NULL;
	home_server *home;
	REALM *realm = NULL;
	home_pool_t *pool;

	/*
	 *	If it was already proxied, do nothing.
	 *
	 *	FIXME: This should really be a serious error.
	 */
	if (request->in_proxy_hash ||
	    (request->proxy_reply && (request->proxy_reply->code != 0))) {
		return 0;
	}

	realmpair = pairfind(request->config_items, PW_PROXY_TO_REALM);
	if (!realmpair || (realmpair->length == 0)) {
		int pool_type;

		vp = pairfind(request->config_items, PW_HOME_SERVER_POOL);
		if (!vp) return 0;

		switch (request->packet->code) {
		case PW_AUTHENTICATION_REQUEST:
			pool_type = HOME_TYPE_AUTH;
			break;

#ifdef WITH_ACCOUNTING
		case PW_ACCOUNTING_REQUEST:
			pool_type = HOME_TYPE_ACCT;
			break;
#endif

#ifdef WITH_COA
		case PW_COA_REQUEST:
		case PW_DISCONNECT_REQUEST:
			pool_type = HOME_TYPE_COA;
			break;
#endif

		default:
			return 0;
		}

		pool = home_pool_byname(vp->vp_strvalue, pool_type);
		if (!pool) {
			RDEBUG2("ERROR: Cannot proxy to unknown pool %s",
				vp->vp_strvalue);
			return 0;
		}

		realmname = NULL; /* no realms */
		realm = NULL;
		goto found_pool;
	}

	realmname = (char *) realmpair->vp_strvalue;

	realm = realm_find2(realmname);
	if (!realm) {
		RDEBUG2("ERROR: Cannot proxy to unknown realm %s", realmname);
		return 0;
	}

	/*
	 *	Figure out which pool to use.
	 */
	if (request->packet->code == PW_AUTHENTICATION_REQUEST) {
		pool = realm->auth_pool;

#ifdef WITH_ACCOUNTING
	} else if (request->packet->code == PW_ACCOUNTING_REQUEST) {
		pool = realm->acct_pool;
#endif

#ifdef WITH_COA
	} else if ((request->packet->code == PW_COA_REQUEST) ||
		   (request->packet->code == PW_DISCONNECT_REQUEST)) {
		pool = realm->acct_pool;
#endif

	} else {
		rad_panic("Internal sanity check failed");
	}

	if (!pool) {
		RDEBUG2(" WARNING: Cancelling proxy to Realm %s, as the realm is local.",
		       realmname);
		return 0;
	}

found_pool:
	home = home_server_ldb(realmname, pool, request);
	if (!home) {
		RDEBUG2("ERROR: Failed to find live home server for realm %s",
		       realmname);
		return -1;
	}
	request->home_pool = pool;

#ifdef WITH_COA
	/*
	 *	Once we've decided to proxy a request, we cannot send
	 *	a CoA packet.  So we free up any CoA packet here.
	 */
	ev_request_free(&request->coa);
#endif
	/*
	 *	Remember that we sent the request to a Realm.
	 */
	if (realmname) pairadd(&request->packet->vps,
			       pairmake("Realm", realmname, T_OP_EQ));

	/*
	 *	Strip the name, if told to.
	 *
	 *	Doing it here catches the case of proxied tunneled
	 *	requests.
	 */
	if (realm && (realm->striprealm == TRUE) &&
	   (strippedname = pairfind(request->proxy->vps, PW_STRIPPED_USER_NAME)) != NULL) {
		/*
		 *	If there's a Stripped-User-Name attribute in
		 *	the request, then use THAT as the User-Name
		 *	for the proxied request, instead of the
		 *	original name.
		 *
		 *	This is done by making a copy of the
		 *	Stripped-User-Name attribute, turning it into
		 *	a User-Name attribute, deleting the
		 *	Stripped-User-Name and User-Name attributes
		 *	from the vps list, and making the new
		 *	User-Name the head of the vps list.
		 */
		vp = pairfind(request->proxy->vps, PW_USER_NAME);
		if (!vp) {
			vp = radius_paircreate(request, NULL,
					       PW_USER_NAME, PW_TYPE_STRING);
			rad_assert(vp != NULL);	/* handled by above function */
			/* Insert at the START of the list */
			vp->next = request->proxy->vps;
			request->proxy->vps = vp;
		}
		memcpy(vp->vp_strvalue, strippedname->vp_strvalue,
		       sizeof(vp->vp_strvalue));
		vp->length = strippedname->length;

		/*
		 *	Do NOT delete Stripped-User-Name.
		 */
	}

	/*
	 *	If there is no PW_CHAP_CHALLENGE attribute but
	 *	there is a PW_CHAP_PASSWORD we need to add it
	 *	since we can't use the request authenticator
	 *	anymore - we changed it.
	 */
	if ((request->packet->code == PW_AUTHENTICATION_REQUEST) &&
	    pairfind(request->proxy->vps, PW_CHAP_PASSWORD) &&
	    pairfind(request->proxy->vps, PW_CHAP_CHALLENGE) == NULL) {
		vp = radius_paircreate(request, &request->proxy->vps,
				       PW_CHAP_CHALLENGE, PW_TYPE_OCTETS);
		vp->length = AUTH_VECTOR_LEN;
		memcpy(vp->vp_strvalue, request->packet->vector, AUTH_VECTOR_LEN);
	}

	/*
	 *	The RFC's say we have to do this, but FreeRADIUS
	 *	doesn't need it.
	 */
	vp = radius_paircreate(request, &request->proxy->vps,
			       PW_PROXY_STATE, PW_TYPE_OCTETS);
	snprintf(vp->vp_strvalue, sizeof(vp->vp_strvalue), "%d",
		 request->packet->id);
	vp->length = strlen(vp->vp_strvalue);

	/*
	 *	Should be done BEFORE inserting into proxy hash, as
	 *	pre-proxy may use this information, or change it.
	 */
	request->proxy->code = request->packet->code;

	/*
	 *	Call the pre-proxy routines.
	 */
	vp = pairfind(request->config_items, PW_PRE_PROXY_TYPE);
	if (vp) {
		RDEBUG2("  Found Pre-Proxy-Type %s", vp->vp_strvalue);
		pre_proxy_type = vp->vp_integer;
	}

	rad_assert(request->home_pool != NULL);

	if (request->home_pool->virtual_server) {
		const char *old_server = request->server;
		
		request->server = request->home_pool->virtual_server;
		RDEBUG2(" server %s {", request->server);
		rcode = module_pre_proxy(pre_proxy_type, request);
		RDEBUG2(" }");
			request->server = old_server;
	} else {
		rcode = module_pre_proxy(pre_proxy_type, request);
	}
	switch (rcode) {
	case RLM_MODULE_FAIL:
	case RLM_MODULE_INVALID:
	case RLM_MODULE_NOTFOUND:
	case RLM_MODULE_USERLOCK:
	default:
		/* FIXME: debug print failed stuff */
		return -1;

	case RLM_MODULE_REJECT:
	case RLM_MODULE_HANDLED:
		return 0;

	/*
	 *	Only proxy the packet if the pre-proxy code succeeded.
	 */
	case RLM_MODULE_NOOP:
	case RLM_MODULE_OK:
	case RLM_MODULE_UPDATED:
		break;
	}

	/*
	 *	If it's a fake request, don't send the proxy
	 *	packet.  The outer tunnel session will take
	 *	care of doing that.
	 */
	if (request->packet->dst_port == 0) {
		request->home_server = NULL;
		return 1;
	}

	if (request->home_server->server) {
		return proxy_to_virtual_server(request);
	}

	if (!proxy_request(request)) {
		RDEBUG("ERROR: Failed to proxy request %u", request->number);
		return -1;
	}
	
	return 1;
}