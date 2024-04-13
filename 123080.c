static void no_response_to_ping(void *ctx)
{
	REQUEST *request = ctx;
	home_server *home;
	char buffer[128];

	rad_assert(request->home_server != NULL);

	home = request->home_server;
	home->num_received_pings = 0;

	radlog(L_ERR, "No response to status check %d for home server %s port %d",
	       request->number,
	       inet_ntop(request->proxy->dst_ipaddr.af,
			 &request->proxy->dst_ipaddr.ipaddr,
			 buffer, sizeof(buffer)),
	       request->proxy->dst_port);

	check_for_zombie_home_server(request);

	wait_for_proxy_id_to_expire(request);
}