pq_getkeepalivescount(Port *port)
{
#ifdef TCP_KEEPCNT
	if (port == NULL || IS_AF_UNIX(port->laddr.addr.ss_family))
		return 0;

	if (port->keepalives_count != 0)
		return port->keepalives_count;

	if (port->default_keepalives_count == 0)
	{
		ACCEPT_TYPE_ARG3 size = sizeof(port->default_keepalives_count);

		if (getsockopt(port->sock, IPPROTO_TCP, TCP_KEEPCNT,
					   (char *) &port->default_keepalives_count,
					   &size) < 0)
		{
			ereport(LOG,
					(errmsg("%s(%s) failed: %m", "getsockopt", "TCP_KEEPCNT")));
			port->default_keepalives_count = -1;	/* don't know */
		}
	}

	return port->default_keepalives_count;
#else
	return 0;
#endif
}