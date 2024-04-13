static void keep_curlalive(CURL *curl)
{
	const long int keepalive = 1;

	curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, keepalive);
	curl_easy_setopt(curl, CURLOPT_TCP_KEEPIDLE, opt_tcp_keepalive);
	curl_easy_setopt(curl, CURLOPT_TCP_KEEPINTVL, opt_tcp_keepalive);
}