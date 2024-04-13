static int keep_sockalive(SOCKETTYPE fd)
{
	const int tcp_keepidle = 60;
	const int tcp_keepintvl = 60;
	const int keepalive = 1;
	int ret = 0;


#ifndef WIN32
	const int tcp_keepcnt = 5;

	if (unlikely(setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &keepalive, sizeof(keepalive))))
		ret = 1;

# ifdef __linux

	if (unlikely(setsockopt(fd, SOL_TCP, TCP_KEEPCNT, &tcp_keepcnt, sizeof(tcp_keepcnt))))
		ret = 1;

	if (unlikely(setsockopt(fd, SOL_TCP, TCP_KEEPIDLE, &tcp_keepidle, sizeof(tcp_keepidle))))
		ret = 1;

	if (unlikely(setsockopt(fd, SOL_TCP, TCP_KEEPINTVL, &tcp_keepintvl, sizeof(tcp_keepintvl))))
		ret = 1;
# endif /* __linux */
# ifdef __APPLE_CC__

	if (unlikely(setsockopt(fd, IPPROTO_TCP, TCP_KEEPALIVE, &tcp_keepintvl, sizeof(tcp_keepintvl))))
		ret = 1;

# endif /* __APPLE_CC__ */

#else /* WIN32 */

	const int zero = 0;
	struct tcp_keepalive vals;
	vals.onoff = 1;
	vals.keepalivetime = tcp_keepidle * 1000;
	vals.keepaliveinterval = tcp_keepintvl * 1000;

	DWORD outputBytes;

	if (unlikely(setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, (const char *)&keepalive, sizeof(keepalive))))
		ret = 1;

	if (unlikely(WSAIoctl(fd, SIO_KEEPALIVE_VALS, &vals, sizeof(vals), NULL, 0, &outputBytes, NULL, NULL)))
		ret = 1;

	/* Windows happily submits indefinitely to the send buffer blissfully
	 * unaware nothing is getting there without gracefully failing unless
	 * we disable the send buffer */
	if (unlikely(setsockopt(fd, SOL_SOCKET, SO_SNDBUF, (const char *)&zero, sizeof(zero))))
		ret = 1;
#endif /* WIN32 */

	return ret;
}