static bool socks5_negotiate(struct pool *pool, int sockd)
{
	unsigned char atyp, uclen;
	unsigned short port;
	char buf[515];
	int i, len;

	buf[0] = 0x05;
	buf[1] = 0x01;
	buf[2] = 0x00;
	applog(LOG_DEBUG, "Attempting to negotiate with %s:%s SOCKS5 proxy",
	       pool->sockaddr_proxy_url, pool->sockaddr_proxy_port );
	send(sockd, buf, 3, 0);
	if (recv_byte(sockd) != 0x05 || recv_byte(sockd) != buf[2]) {
		applog(LOG_WARNING, "Bad response from %s:%s SOCKS5 server",
		       pool->sockaddr_proxy_url, pool->sockaddr_proxy_port );
		return false;
	}

	buf[0] = 0x05;
	buf[1] = 0x01;
	buf[2] = 0x00;
	buf[3] = 0x03;
	len = (strlen(pool->sockaddr_url));
	if (len > 255)
		len = 255;
	uclen = len;
	buf[4] = (uclen & 0xff);
	memcpy(buf + 5, pool->sockaddr_url, len);
	port = atoi(pool->stratum_port);
	buf[5 + len] = (port >> 8);
	buf[6 + len] = (port & 0xff);
	send(sockd, buf, (7 + len), 0);
	if (recv_byte(sockd) != 0x05 || recv_byte(sockd) != 0x00) {
		applog(LOG_WARNING, "Bad response from %s:%s SOCKS5 server",
			pool->sockaddr_proxy_url, pool->sockaddr_proxy_port );
		return false;
	}

	recv_byte(sockd);
	atyp = recv_byte(sockd);
	if (atyp == 0x01) {
		for (i = 0; i < 4; i++)
			recv_byte(sockd);
	} else if (atyp == 0x03) {
		len = recv_byte(sockd);
		for (i = 0; i < len; i++)
			recv_byte(sockd);
	} else {
		applog(LOG_WARNING, "Bad response from %s:%s SOCKS5 server",
			pool->sockaddr_proxy_url, pool->sockaddr_proxy_port );
		return false;
	}
	for (i = 0; i < 2; i++)
		recv_byte(sockd);

	applog(LOG_DEBUG, "Success negotiating with %s:%s SOCKS5 proxy",
	       pool->sockaddr_proxy_url, pool->sockaddr_proxy_port);
	return true;
}