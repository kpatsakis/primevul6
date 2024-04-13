int json_rpc_call_sockopt_cb(void __maybe_unused *userdata, curl_socket_t fd,
			     curlsocktype __maybe_unused purpose)
{
	return keep_sockalive(fd);
}