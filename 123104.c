void revive_home_server(void *ctx)
{
	home_server *home = ctx;
	char buffer[128];

	home->state = HOME_STATE_ALIVE;
	home->currently_outstanding = 0;
	home->revive_time = now;

	/*
	 *	Delete any outstanding events.
	 */
	if (home->ev) fr_event_delete(el, &home->ev);

	radlog(L_PROXY, "Marking home server %s port %d alive again... we have no idea if it really is alive or not.",
	       inet_ntop(home->ipaddr.af, &home->ipaddr.ipaddr,
			 buffer, sizeof(buffer)),
	       home->port);

}