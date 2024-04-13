int thread_pool_addrequest(REQUEST *request, RAD_REQUEST_FUNP fun)
{
	radius_handle_request(request, fun);
	return 1;
}