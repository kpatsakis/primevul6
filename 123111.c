static void proxy_fallback_handler(REQUEST *request)
{
	/*
	 *	A proper time is required for wait_a_bit.
	 */
	request->delay = USEC / 10;
	gettimeofday(&now, NULL);
	request->next_when = now;
	tv_add(&request->next_when, request->delay);
	request->next_callback = wait_a_bit;

	/*
	 *	Re-queue the request.
	 */
	request->child_state = REQUEST_QUEUED;
	
	rad_assert(request->proxy != NULL);
	if (!thread_pool_addrequest(request, virtual_server_handler)) {
		request->child_state = REQUEST_DONE;
	}

#ifdef HAVE_PTHREAD_H
	/*
	 *	MAY free the request if we're over max_request_time,
	 *	AND we're not in threaded mode!
	 *
	 *	Note that we call this ONLY if we're threaded, as
	 *	if we're NOT threaded, request_post_handler() calls
	 *	wait_a_bit(), which means that "request" may not
	 *	exist any more...
	 */
	if (have_children) wait_a_bit(request);
#endif
}