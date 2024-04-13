pq_setkeepalivesidle(int idle, Port *port)
{
	if (port == NULL || IS_AF_UNIX(port->laddr.addr.ss_family))
		return STATUS_OK;

/* check SIO_KEEPALIVE_VALS here, not just WIN32, as some toolchains lack it */
#if defined(PG_TCP_KEEPALIVE_IDLE) || defined(SIO_KEEPALIVE_VALS)
	if (idle == port->keepalives_idle)
		return STATUS_OK;

#ifndef WIN32
	if (port->default_keepalives_idle <= 0)
	{
		if (pq_getkeepalivesidle(port) < 0)
		{
			if (idle == 0)
				return STATUS_OK;	/* default is set but unknown */
			else
				return STATUS_ERROR;
		}
	}

	if (idle == 0)
		idle = port->default_keepalives_idle;

	if (setsockopt(port->sock, IPPROTO_TCP, PG_TCP_KEEPALIVE_IDLE,
				   (char *) &idle, sizeof(idle)) < 0)
	{
		ereport(LOG,
				(errmsg("%s(%s) failed: %m", "setsockopt", PG_TCP_KEEPALIVE_IDLE_STR)));
		return STATUS_ERROR;
	}

	port->keepalives_idle = idle;
#else							/* WIN32 */
	return pq_setkeepaliveswin32(port, idle, port->keepalives_interval);
#endif
#else
	if (idle != 0)
	{
		ereport(LOG,
				(errmsg("setting the keepalive idle time is not supported")));
		return STATUS_ERROR;
	}
#endif

	return STATUS_OK;
}