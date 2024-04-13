static void cleanup_delay(void *ctx)
{
	REQUEST *request = ctx;

	rad_assert(request->magic == REQUEST_MAGIC);
	rad_assert((request->child_state == REQUEST_CLEANUP_DELAY) ||
		   (request->child_state == REQUEST_DONE));

	remove_from_request_hash(request);

#ifdef WITH_PROXY
	if (request->proxy && request->in_proxy_hash) {
		wait_for_proxy_id_to_expire(request);
		return;
	}
#endif

	RDEBUG2("Cleaning up request %u ID %d with timestamp +%d",
	       request->number, request->packet->id,
	       (unsigned int) (request->timestamp - fr_start_time));

	ev_request_free(&request);
}