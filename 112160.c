pq_settcpusertimeout(int timeout, Port *port)
{
	if (port == NULL || IS_AF_UNIX(port->laddr.addr.ss_family))
		return STATUS_OK;

#ifdef TCP_USER_TIMEOUT
	if (timeout == port->tcp_user_timeout)
		return STATUS_OK;

	if (port->default_tcp_user_timeout <= 0)
	{
		if (pq_gettcpusertimeout(port) < 0)
		{
			if (timeout == 0)
				return STATUS_OK;	/* default is set but unknown */
			else
				return STATUS_ERROR;
		}
	}

	if (timeout == 0)
		timeout = port->default_tcp_user_timeout;

	if (setsockopt(port->sock, IPPROTO_TCP, TCP_USER_TIMEOUT,
				   (char *) &timeout, sizeof(timeout)) < 0)
	{
		ereport(LOG,
				(errmsg("%s(%s) failed: %m", "setsockopt", "TCP_USER_TIMEOUT")));
		return STATUS_ERROR;
	}

	port->tcp_user_timeout = timeout;
#else
	if (timeout != 0)
	{
		ereport(LOG,
				(errmsg("%s(%s) not supported", "setsockopt", "TCP_USER_TIMEOUT")));
		return STATUS_ERROR;
	}
#endif

	return STATUS_OK;
}