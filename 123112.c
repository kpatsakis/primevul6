static void ping_home_server(void *ctx)
{
	uint32_t jitter;
	home_server *home = ctx;
	REQUEST *request;
	VALUE_PAIR *vp;

	if ((home->state == HOME_STATE_ALIVE) ||
	    (home->ping_check == HOME_PING_CHECK_NONE) ||
	    (home->ev != NULL)) {
		return;
	}

	request = request_alloc();
	request->number = request_num_counter++;

	request->proxy = rad_alloc(1);
	rad_assert(request->proxy != NULL);

	fr_event_now(el, &request->when);
	home->when = request->when;

	if (home->ping_check == HOME_PING_CHECK_STATUS_SERVER) {
		request->proxy->code = PW_STATUS_SERVER;

		radius_pairmake(request, &request->proxy->vps,
				"Message-Authenticator", "0x00", T_OP_SET);

	} else if (home->type == HOME_TYPE_AUTH) {
		request->proxy->code = PW_AUTHENTICATION_REQUEST;

		radius_pairmake(request, &request->proxy->vps,
				"User-Name", home->ping_user_name, T_OP_SET);
		radius_pairmake(request, &request->proxy->vps,
				"User-Password", home->ping_user_password, T_OP_SET);
		radius_pairmake(request, &request->proxy->vps,
				"Service-Type", "Authenticate-Only", T_OP_SET);
		radius_pairmake(request, &request->proxy->vps,
				"Message-Authenticator", "0x00", T_OP_SET);

	} else {
#ifdef WITH_ACCOUNTING
		request->proxy->code = PW_ACCOUNTING_REQUEST;
		
		radius_pairmake(request, &request->proxy->vps,
				"User-Name", home->ping_user_name, T_OP_SET);
		radius_pairmake(request, &request->proxy->vps,
				"Acct-Status-Type", "Stop", T_OP_SET);
		radius_pairmake(request, &request->proxy->vps,
				"Acct-Session-Id", "00000000", T_OP_SET);
		vp = radius_pairmake(request, &request->proxy->vps,
				     "Event-Timestamp", "0", T_OP_SET);
		vp->vp_date = now.tv_sec;
#else
		rad_assert("Internal sanity check failed");
#endif
	}

	radius_pairmake(request, &request->proxy->vps,
			"NAS-Identifier", "Status Check. Are you alive?",
			T_OP_SET);

	request->proxy->dst_ipaddr = home->ipaddr;
	request->proxy->dst_port = home->port;
	request->home_server = home;

	rad_assert(request->proxy_listener == NULL);

	if (!insert_into_proxy_hash(request, FALSE)) {
		RDEBUG2("ERROR: Failed inserting status check %d into proxy hash.  Discarding it.",
		       request->number);
		ev_request_free(&request);
		return;
	}
	rad_assert(request->proxy_listener != NULL);
	request->proxy_listener->send(request->proxy_listener,
				      request);

	request->next_callback = NULL;
	request->child_state = REQUEST_PROXIED;
	gettimeofday(&request->when, NULL);
	home->when = request->when;
	request->when.tv_sec += home->ping_timeout;;

	INSERT_EVENT(no_response_to_ping, request);

	/*
	 *	Add +/- 2s of jitter, as suggested in RFC 3539
	 *	and in the Issues and Fixes draft.
	 */
	home->when.tv_sec += home->ping_interval - 2;

	jitter = fr_rand();
	jitter ^= (jitter >> 10);
	jitter &= ((1 << 23) - 1); /* 22 bits of 1 */

	tv_add(&home->when, jitter);

	INSERT_EVENT(ping_home_server, home);
}