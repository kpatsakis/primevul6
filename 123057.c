static int virtual_server_handler(UNUSED REQUEST *request)
{
	proxy_to_virtual_server(request);
	return 0;
}