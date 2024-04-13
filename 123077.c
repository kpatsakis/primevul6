static void retransmit_coa_request(void *ctx)
{
	int delay, frac;
	struct timeval mrd;
	REQUEST *request = ctx;

	rad_assert(request->magic == REQUEST_MAGIC);
	rad_assert(request->child_state == REQUEST_PROXIED);
	rad_assert(request->home_server != NULL);
	rad_assert(!request->in_request_hash);
	rad_assert(request->parent == NULL);
	
	fr_event_now(el, &now);

	/*
	 *	Cap count at MRC, if it is non-zero.
	 */
	if (request->home_server->coa_mrc &&
	    (request->num_coa_requests >= request->home_server->coa_mrc)) {
		no_response_to_coa_request(request);
		return;
	}

	/*
	 *	RFC 5080 Section 2.2.1
	 *
	 *	RT = 2*RTprev + RAND*RTprev
	 *	   = 1.9 * RTprev + rand(0,.2) * RTprev
	 *	   = 1.9 * RTprev + rand(0,1) * (RTprev / 5)
	 */
	delay = fr_rand();
	delay ^= (delay >> 16);
	delay &= 0xffff;
	frac = request->delay / 5;
	delay = ((frac >> 16) * delay) + (((frac & 0xffff) * delay) >> 16);

	delay += (2 * request->delay) - (request->delay / 10);

	/*
	 *	Cap delay at MRT, if MRT is non-zero.
	 */
	if (request->home_server->coa_mrt &&
	    (delay > (request->home_server->coa_mrt * USEC))) {
		int mrt_usec = request->home_server->coa_mrt * USEC;

		/*
		 *	delay = MRT + RAND * MRT
		 *	      = 0.9 MRT + rand(0,.2)  * MRT
		 */
		delay = fr_rand();
		delay ^= (delay >> 15);
		delay &= 0x1ffff;
		delay = ((mrt_usec >> 16) * delay) + (((mrt_usec & 0xffff) * delay) >> 16);
		delay += mrt_usec - (mrt_usec / 10);
	}

	request->delay = delay;
	request->when = now;
	tv_add(&request->when, request->delay);
	mrd = request->proxy_when;
	mrd.tv_sec += request->home_server->coa_mrd;

	/*
	 *	Cap duration at MRD.
	 */
	if (timercmp(&mrd, &request->when, <)) {
		request->when = mrd;
		INSERT_EVENT(no_response_to_coa_request, request);

	} else {
		INSERT_EVENT(retransmit_coa_request, request);
	}
	
	if (update_event_timestamp(request->proxy, now.tv_sec)) {
		if (!insert_into_proxy_hash(request, TRUE)) {
			DEBUG("ERROR: Failed re-inserting CoA request into proxy hash.");
			return;
		}

		request->num_proxied_requests = 0;
		request->num_proxied_responses = 0;
	}

	request->num_proxied_requests++;
	request->num_coa_requests++; /* is NOT reset by code 3 lines above! */

	request->proxy_listener->send(request->proxy_listener,
				      request);
}