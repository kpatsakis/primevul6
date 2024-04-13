static void no_response_to_proxied_request(void *ctx)
{
	REQUEST *request = ctx;
	home_server *home;
	char buffer[128];

	rad_assert(request->magic == REQUEST_MAGIC);
	rad_assert(request->child_state == REQUEST_PROXIED);

	/*
	 *	If we've failed over to an internal home server,
	 *	replace the callback with the correct one.  This
	 *	is due to locking issues with child threads...
	 */
	if (request->home_server->server) {
		wait_a_bit(request);
		return;
	}

	check_for_zombie_home_server(request);

	home = request->home_server;

	/*
	 *	The default as of 2.1.7 is to allow requests to
	 *	fail-over to a backup home server when this one does
	 *	not respond.  The old behavior can be configured as
	 *	well.
	 */
	if (home->no_response_fail) {
		radlog(L_ERR, "Rejecting request %u (proxy Id %d) due to lack of any response from home server %s port %d",
		       request->number, request->proxy->id,
		       inet_ntop(request->proxy->dst_ipaddr.af,
				 &request->proxy->dst_ipaddr.ipaddr,
				 buffer, sizeof(buffer)),
		       request->proxy->dst_port);

		post_proxy_fail_handler(request);
	} else {
		rad_assert(request->ev == NULL);
		request->child_state = REQUEST_RUNNING;
		wait_a_bit(request);
	}

	/*
	 *	Don't touch request due to race conditions
	 */
	if (home->state == HOME_STATE_IS_DEAD) {
		rad_assert(home->ev != NULL); /* or it will never wake up */
		return;
	}

	/*
	 *	Enable the zombie period when we notice that the home
	 *	server hasn't responded.  We do NOT back-date the start
	 *	of the zombie period.
	 */
	if (home->state == HOME_STATE_ALIVE) {
		home->state = HOME_STATE_ZOMBIE;
		home->zombie_period_start = now;	
		fr_event_delete(el, &home->ev);
		home->currently_outstanding = 0;
		home->num_received_pings = 0;

		radlog(L_PROXY, "Marking home server %s port %d as zombie (it looks like it is dead).",
		       inet_ntop(home->ipaddr.af, &home->ipaddr.ipaddr,
				 buffer, sizeof(buffer)),
		       home->port);

		/*
		 *	Start pinging the home server.
		 */
		ping_home_server(home);
	}
}