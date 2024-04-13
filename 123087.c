static void check_for_zombie_home_server(REQUEST *request)
{
	home_server *home;
	struct timeval when;

	home = request->home_server;

	if (home->state != HOME_STATE_ZOMBIE) return;

	when = home->zombie_period_start;
	when.tv_sec += home->zombie_period;

	fr_event_now(el, &now);
	if (timercmp(&now, &when, <)) {
		return;
	}

	mark_home_server_dead(home, &request->when);
}