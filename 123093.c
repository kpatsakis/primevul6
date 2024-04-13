void mark_home_server_dead(home_server *home, struct timeval *when)
{
	int previous_state = home->state;
	char buffer[128];

	radlog(L_PROXY, "Marking home server %s port %d as dead.",
	       inet_ntop(home->ipaddr.af, &home->ipaddr.ipaddr,
			 buffer, sizeof(buffer)),
	       home->port);

	home->state = HOME_STATE_IS_DEAD;
	home->num_received_pings = 0;

	if (home->ping_check != HOME_PING_CHECK_NONE) {
		/*
		 *	If the control socket marks us dead, start
		 *	pinging.  Otherwise, we already started
		 *	pinging when it was marked "zombie".
		 */
		if (previous_state == HOME_STATE_ALIVE) {
			ping_home_server(home);
		}

	} else {
		/*
		 *	Revive it after a fixed period of time.  This
		 *	is very, very, bad.
		 */
		home->when = *when;
		home->when.tv_sec += home->revive_interval;

		INSERT_EVENT(revive_home_server, home);
	}
}