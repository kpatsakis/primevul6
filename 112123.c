pq_getkeepalivesinterval(Port *port)
{
#if defined(TCP_KEEPINTVL) || defined(SIO_KEEPALIVE_VALS)
	if (port == NULL || IS_AF_UNIX(port->laddr.addr.ss_family))
		return 0;

	if (port->keepalives_interval != 0)
		return port->keepalives_interval;

	if (port->default_keepalives_interval == 0)
	{
#ifndef WIN32
		ACCEPT_TYPE_ARG3 size = sizeof(port->default_keepalives_interval);

		if (getsockopt(port->sock, IPPROTO_TCP, TCP_KEEPINTVL,
					   (char *) &port->default_keepalives_interval,
					   &size) < 0)
		{
			ereport(LOG,
					(errmsg("%s(%s) failed: %m", "getsockopt", "TCP_KEEPINTVL")));
			port->default_keepalives_interval = -1; /* don't know */
		}
#else
		/* We can't get the defaults on Windows, so return "don't know" */
		port->default_keepalives_interval = -1;
#endif							/* WIN32 */
	}

	return port->default_keepalives_interval;
#else
	return 0;
#endif
}