static void reject_delay(void *ctx)
{
	REQUEST *request = ctx;

	rad_assert(request->magic == REQUEST_MAGIC);
	rad_assert(request->child_state == REQUEST_REJECT_DELAY);

	RDEBUG2("Sending delayed reject for request %u", request->number);

	DEBUG_PACKET(request, request->reply, 1);

	request->listener->send(request->listener, request);

	request->when.tv_sec += request->root->cleanup_delay;
	request->child_state = REQUEST_CLEANUP_DELAY;

	INSERT_EVENT(cleanup_delay, request);
}