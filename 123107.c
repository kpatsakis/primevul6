static int proxy_hash_cb(UNUSED void *ctx, void *data)
{
	REQUEST *request = fr_packet2myptr(REQUEST, proxy, data);

	ev_request_free(&request);

	return 0;
}