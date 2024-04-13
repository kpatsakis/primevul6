static void post_proxy_fail_handler(REQUEST *request)
{
	/*
	 *	A proper time is required for wait_a_bit.
	 */
	request->delay = USEC / 10;
	gettimeofday(&now, NULL);

	/*
	 *	Not set up to run Post-Proxy-Type = Fail.
	 *
	 *	Mark the request as still running, and figure out what
	 *	to do next.
	 */
	if (!setup_post_proxy_fail(request)) {
		request_post_handler(request);

	} else {
		/*
		 *	Re-queue the request.
		 */
		request->child_state = REQUEST_QUEUED;

		/*
		 *	There is a post-proxy-type of fail.  We run
		 *	the request through the pre/post proxy
		 *	handlers, just like it was a real proxied
		 *	request.  However, we set the per-request
		 *	handler to NULL, as we don't want to do
		 *	anything else.
		 *
		 *	Note that when we're not threaded, this will
		 *	process the request even if it's greater than
		 *	max_request_time.  That's not fatal.
		 */
		request->priority = 0;
		rad_assert(request->proxy != NULL);
		thread_pool_addrequest(request, null_handler);
	}

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
}