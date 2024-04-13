pq_setkeepalivescount(int count, Port *port)
{
	if (port == NULL || IS_AF_UNIX(port->laddr.addr.ss_family))
		return STATUS_OK;

#ifdef TCP_KEEPCNT
	if (count == port->keepalives_count)
		return STATUS_OK;

	if (port->default_keepalives_count <= 0)
	{
		if (pq_getkeepalivescount(port) < 0)
		{
			if (count == 0)
				return STATUS_OK;	/* default is set but unknown */
			else
				return STATUS_ERROR;
		}
	}

	if (count == 0)
		count = port->default_keepalives_count;

	if (setsockopt(port->sock, IPPROTO_TCP, TCP_KEEPCNT,
				   (char *) &count, sizeof(count)) < 0)
	{
		ereport(LOG,
				(errmsg("%s(%s) failed: %m", "setsockopt", "TCP_KEEPCNT")));
		return STATUS_ERROR;
	}

	port->keepalives_count = count;
#else
	if (count != 0)
	{
		ereport(LOG,
				(errmsg("%s(%s) not supported", "setsockopt", "TCP_KEEPCNT")));
		return STATUS_ERROR;
	}
#endif

	return STATUS_OK;
}