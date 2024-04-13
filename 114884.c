static bool parse_reconnect(struct pool *pool, json_t *val)
{
	char *url, *port, address[256];

	memset(address, 0, 255);
	url = (char *)json_string_value(json_array_get(val, 0));
	if (!url)
		url = pool->sockaddr_url;

	port = (char *)json_string_value(json_array_get(val, 1));
	if (!port)
		port = pool->stratum_port;

	sprintf(address, "%s:%s", url, port);

	if (!extract_sockaddr(pool, address))
		return false;

	pool->stratum_url = pool->sockaddr_url;

	applog(LOG_NOTICE, "Reconnect requested from pool %d to %s", pool->pool_no, address);

	if (!restart_stratum(pool))
		return false;

	return true;
}