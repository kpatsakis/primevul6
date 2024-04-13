static void no_response_to_coa_request(void *ctx)
{
	REQUEST *request = ctx;
	char buffer[128];

	rad_assert(request->magic == REQUEST_MAGIC);
	rad_assert(request->child_state == REQUEST_PROXIED);
	rad_assert(request->home_server != NULL);
	rad_assert(!request->in_request_hash);

	radlog(L_ERR, "No response to CoA request sent to %s",
	       inet_ntop(request->proxy->dst_ipaddr.af,
			 &request->proxy->dst_ipaddr.ipaddr,
			 buffer, sizeof(buffer)));

	/*
	 *	Hack.
	 */
	request->packet->code |= (PW_COA_REQUEST << 8);
	post_proxy_fail_handler(request);
}