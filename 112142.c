pq_gettcpusertimeout(Port *port)
{
#ifdef TCP_USER_TIMEOUT
	if (port == NULL || IS_AF_UNIX(port->laddr.addr.ss_family))
		return 0;

	if (port->tcp_user_timeout != 0)
		return port->tcp_user_timeout;

	if (port->default_tcp_user_timeout == 0)
	{
		ACCEPT_TYPE_ARG3 size = sizeof(port->default_tcp_user_timeout);

		if (getsockopt(port->sock, IPPROTO_TCP, TCP_USER_TIMEOUT,
					   (char *) &port->default_tcp_user_timeout,
					   &size) < 0)
		{
			ereport(LOG,
					(errmsg("%s(%s) failed: %m", "getsockopt", "TCP_USER_TIMEOUT")));
			port->default_tcp_user_timeout = -1;	/* don't know */
		}
	}

	return port->default_tcp_user_timeout;
#else
	return 0;
#endif
}