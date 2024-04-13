static void wait_for_proxy_id_to_expire(void *ctx)
{
	REQUEST *request = ctx;

	rad_assert(request->magic == REQUEST_MAGIC);
	rad_assert(request->proxy != NULL);

	if (!fr_event_now(el, &now)) gettimeofday(&now, NULL);
	request->when = request->proxy_when;

#ifdef WITH_COA
	if (((request->proxy->code == PW_COA_REQUEST) ||
	     (request->proxy->code == PW_DISCONNECT_REQUEST)) &&
	    (request->packet->code != request->proxy->code)) {
		request->when.tv_sec += request->home_server->coa_mrd;
	} else
#endif
	request->when.tv_sec += request->home_server->response_window;

	if ((request->num_proxied_requests == request->num_proxied_responses) ||
	    timercmp(&now, &request->when, >)) {
		if (request->packet) {
			RDEBUG2("Cleaning up request %u ID %d with timestamp +%d",
			       request->number, request->packet->id,
			       (unsigned int) (request->timestamp - fr_start_time));
		} else {
			RDEBUG2("Cleaning up request %u with timestamp +%d",
			       request->number,
			       (unsigned int) (request->timestamp - fr_start_time));
		}

		ev_request_free(&request);
		return;
	}

	INSERT_EVENT(wait_for_proxy_id_to_expire, request);
}