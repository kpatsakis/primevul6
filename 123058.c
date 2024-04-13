static void request_post_handler(REQUEST *request)
{
	int child_state = -1;
	struct timeval when;
	VALUE_PAIR *vp;

	if ((request->master_state == REQUEST_STOP_PROCESSING) ||
	    (request->parent &&
	     (request->parent->master_state == REQUEST_STOP_PROCESSING))) {
		RDEBUG2("request %u was cancelled.", request->number);
#ifdef HAVE_PTHREAD_H
		request->child_pid = NO_SUCH_CHILD_PID;
#endif
		child_state = REQUEST_DONE;
		goto cleanup;
	}

	if (request->child_state != REQUEST_RUNNING) {
		rad_panic("Internal sanity check failed");
	}

#ifdef WITH_COA
	/*
	 *	If it's not in the request hash, it's a CoA request.
	 *	We hope.
	 */
	if (!request->in_request_hash &&
	    request->proxy &&
	    ((request->proxy->code == PW_COA_REQUEST) ||
	     (request->proxy->code == PW_DISCONNECT_REQUEST))) {
		request->next_callback = NULL;
		child_state = REQUEST_DONE;
		goto cleanup;
	}
#endif

	/*
	 *	Catch Auth-Type := Reject BEFORE proxying the packet.
	 */
	if ((request->packet->code == PW_AUTHENTICATION_REQUEST) &&
	    (request->reply->code == 0) &&
	    ((vp = pairfind(request->config_items, PW_AUTH_TYPE)) != NULL) &&
	    (vp->vp_integer == PW_AUTHTYPE_REJECT)) {
		request->reply->code = PW_AUTHENTICATION_REJECT;
	}

#ifdef WITH_PROXY
	if (request->root->proxy_requests &&
	    !request->in_proxy_hash &&
	    (request->reply->code == 0) &&
	    (request->packet->dst_port != 0) &&
	    (request->packet->code != PW_STATUS_SERVER)) {
		int rcode = successfully_proxied_request(request);

		if (rcode == 1) return; /* request is invalid */

		/*
		 *	Failed proxying it (dead home servers, etc.)
		 *	Run it through Post-Proxy-Type = Fail, and
		 *	respond to the request.
		 *
		 *	Note that we're in a child thread here, so we
		 *	do NOT re-schedule the request.  Instead, we
		 *	do what we would have done, which is run the
		 *	pre-handler, a NULL request handler, and then
		 *	the post handler.
		 */
		if ((rcode < 0) && setup_post_proxy_fail(request)) {
			request_pre_handler(request);
		}

		/*
		 *	Else we weren't supposed to proxy it,
		 *	OR we proxied it internally to a virutal server.
		 */
	}

#ifdef WITH_COA
	else if (request->proxy && request->coa) {
		RDEBUG("WARNING: Cannot proxy and originate CoA packets at the same time.  Cancelling CoA request");
		ev_request_free(&request->coa);
	}
#endif
#endif

	/*
	 *	Fake requests don't get encoded or signed.  The caller
	 *	also requires the reply VP's, so we don't free them
	 *	here!
	 */
	if (request->packet->dst_port == 0) {
		/* FIXME: RDEBUG going to the next request */
#ifdef HAVE_PTHREAD_H
		request->child_pid = NO_SUCH_CHILD_PID;
#endif
		request->child_state = REQUEST_DONE;
		return;
	}

#ifdef WITH_PROXY
	/*
	 *	Copy Proxy-State from the request to the reply.
	 */
	vp = paircopy2(request->packet->vps, PW_PROXY_STATE);
	if (vp) pairadd(&request->reply->vps, vp);
#endif

	/*
	 *	Access-Requests get delayed or cached.
	 */
	switch (request->packet->code) {
	case PW_AUTHENTICATION_REQUEST:
		gettimeofday(&request->next_when, NULL);

		if (request->reply->code == 0) {
			/*
			 *	Check if the lack of response is intentional.
			 */
			vp = pairfind(request->config_items,
				      PW_RESPONSE_PACKET_TYPE);
			if (!vp) {
				RDEBUG2("There was no response configured: rejecting request %u",
				       request->number);
				request->reply->code = PW_AUTHENTICATION_REJECT;

			} else if (vp->vp_integer == 256) {
				RDEBUG2("Not responding to request %u",
				       request->number);

				/*
				 *	Force cleanup after a long
				 *	time, so that we don't
				 *	re-process the packet.
				 */
				request->next_when.tv_sec += request->root->max_request_time;
				request->next_callback = cleanup_delay;
				child_state = REQUEST_CLEANUP_DELAY;
				break;
			} else {
				request->reply->code = vp->vp_integer;

			}
		}

		/*
		 *	Run rejected packets through
		 *
		 *	Post-Auth-Type = Reject
		 */
		if (request->reply->code == PW_AUTHENTICATION_REJECT) {
			pairdelete(&request->config_items, PW_POST_AUTH_TYPE);
			vp = radius_pairmake(request, &request->config_items,
					     "Post-Auth-Type", "Reject",
					     T_OP_SET);
			if (vp) rad_postauth(request);

			/*
			 *	If configured, delay Access-Reject packets.
			 *
			 *	If request->root->reject_delay = 0, we discover
			 *	that we have to send the packet now.
			 */
			when = request->received;
			when.tv_sec += request->root->reject_delay;

			if (timercmp(&when, &request->next_when, >)) {
				RDEBUG2("Delaying reject of request %u for %d seconds",
				       request->number,
				       request->root->reject_delay);
				request->next_when = when;
				request->next_callback = reject_delay;
#ifdef HAVE_PTHREAD_H
				request->child_pid = NO_SUCH_CHILD_PID;
#endif
				request->child_state = REQUEST_REJECT_DELAY;
				return;
			}
		}

#ifdef WITH_COA
	case PW_COA_REQUEST:
	case PW_DISCONNECT_REQUEST:
#endif
		request->next_when.tv_sec += request->root->cleanup_delay;
		request->next_callback = cleanup_delay;
		child_state = REQUEST_CLEANUP_DELAY;
		break;

	case PW_ACCOUNTING_REQUEST:
		request->next_callback = NULL; /* just to be safe */
		child_state = REQUEST_DONE;
		break;

		/*
		 *	FIXME: Status-Server should probably not be
		 *	handled here...
		 */
	case PW_STATUS_SERVER:
		request->next_callback = NULL;
		child_state = REQUEST_DONE;
		break;

	default:
		/*
		 *	DHCP, VMPS, etc.
		 */
		request->next_callback = NULL;
		child_state = REQUEST_DONE;
		break;
	}

	/*
	 *      Suppress "no reply" packets here, unless we're reading
	 *      from the "detail" file.  In that case, we've got to
	 *      tell the detail file handler that the request is dead,
	 *      and it should re-send it.
	 *	If configured, encode, sign, and send.
	 */
	if ((request->reply->code != 0) ||
	    (request->listener->type == RAD_LISTEN_DETAIL)) {
		DEBUG_PACKET(request, request->reply, 1);
		request->listener->send(request->listener, request);
	}

#ifdef WITH_COA
	/*
	 *	Now that we've completely processed the request,
	 *	see if we need to originate a CoA request.  But ONLY
	 *	if it wasn't proxied.
	 */
	if (!request->proxy &&
	    (request->coa ||
	     (pairfind(request->config_items, PW_SEND_COA_REQUEST) != NULL))) {
		if (!originated_coa_request(request)) {
			RDEBUG2("Do CoA Fail handler here");
		}
		/* request->coa is stil set, so we can update events */
	}
#endif

 cleanup:
	/*
	 *	Clean up.  These are no longer needed.
	 */
	pairfree(&request->config_items);

	pairfree(&request->packet->vps);
	request->username = NULL;
	request->password = NULL;

	pairfree(&request->reply->vps);

#ifdef WITH_PROXY
	if (request->proxy) {
		pairfree(&request->proxy->vps);

		if (request->proxy_reply) {
			pairfree(&request->proxy_reply->vps);
		}

#if 0
		/*
		 *	We're not tracking responses from the home
		 *	server, we can therefore free this memory in
		 *	the child thread.
		 */
		if (!request->in_proxy_hash) {
			rad_free(&request->proxy);
			rad_free(&request->proxy_reply);
			request->home_server = NULL;
		}
#endif
	}
#endif

	RDEBUG2("Finished request %u.", request->number);
	rad_assert(child_state >= 0);
	request->child_state = child_state;

	/*
	 *	Single threaded mode: update timers now.
	 */
	if (!have_children) wait_a_bit(request);
}