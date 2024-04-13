static void event_socket_handler(fr_event_list_t *xel, UNUSED int fd,
				 void *ctx)
{
	rad_listen_t *listener = ctx;
	RAD_REQUEST_FUNP fun;
	REQUEST *request;

	rad_assert(xel == el);

	xel = xel;

	if (listener->fd < 0) rad_panic("Socket was closed on us!");
	
	if (!listener->recv(listener, &fun, &request)) return;

	if (!thread_pool_addrequest(request, fun)) {
		request->child_state = REQUEST_DONE;
	}
}