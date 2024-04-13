void radius_handle_request(REQUEST *request, RAD_REQUEST_FUNP fun)
{
	request->options = RAD_REQUEST_OPTION_DEBUG2;

	if (request_pre_handler(request)) {
		rad_assert(fun != NULL);
		rad_assert(request != NULL);
		
		if (request->server) RDEBUG("server %s {",
					    request->server != NULL ?
					    request->server : ""); 
		fun(request);

		if (request->server) RDEBUG("} # server %s",
					     request->server != NULL ?
					    request->server : "");

		request_post_handler(request);
	}

	DEBUG2("Going to the next request");
	return;
}