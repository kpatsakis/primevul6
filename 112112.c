pq_getkeepalivesidle(Port *port)
{
#if defined(PG_TCP_KEEPALIVE_IDLE) || defined(SIO_KEEPALIVE_VALS)
	if (port == NULL || IS_AF_UNIX(port->laddr.addr.ss_family))
		return 0;

	if (port->keepalives_idle != 0)
		return port->keepalives_idle;

	if (port->default_keepalives_idle == 0)
	{
#ifndef WIN32
		ACCEPT_TYPE_ARG3 size = sizeof(port->default_keepalives_idle);

		if (getsockopt(port->sock, IPPROTO_TCP, PG_TCP_KEEPALIVE_IDLE,
					   (char *) &port->default_keepalives_idle,
					   &size) < 0)
		{
			ereport(LOG,
					(errmsg("%s(%s) failed: %m", "getsockopt", PG_TCP_KEEPALIVE_IDLE_STR)));
			port->default_keepalives_idle = -1; /* don't know */
		}
#else							/* WIN32 */
		/* We can't get the defaults on Windows, so return "don't know" */
		port->default_keepalives_idle = -1;
#endif							/* WIN32 */
	}

	return port->default_keepalives_idle;
#else
	return 0;
#endif
}