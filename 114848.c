curl_socket_t grab_socket_opensocket_cb(void *clientp, __maybe_unused curlsocktype purpose, struct curl_sockaddr *addr)
{
	struct pool *pool = clientp;
	curl_socket_t sck = socket(addr->family, addr->socktype, addr->protocol);
	pool->sock = sck;
	return sck;
}