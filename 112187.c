pq_setkeepaliveswin32(Port *port, int idle, int interval)
{
	struct tcp_keepalive ka;
	DWORD		retsize;

	if (idle <= 0)
		idle = 2 * 60 * 60;		/* default = 2 hours */
	if (interval <= 0)
		interval = 1;			/* default = 1 second */

	ka.onoff = 1;
	ka.keepalivetime = idle * 1000;
	ka.keepaliveinterval = interval * 1000;

	if (WSAIoctl(port->sock,
				 SIO_KEEPALIVE_VALS,
				 (LPVOID) &ka,
				 sizeof(ka),
				 NULL,
				 0,
				 &retsize,
				 NULL,
				 NULL)
		!= 0)
	{
		ereport(LOG,
				(errmsg("%s(%s) failed: error code %d",
						"WSAIoctl", "SIO_KEEPALIVE_VALS", WSAGetLastError())));
		return STATUS_ERROR;
	}
	if (port->keepalives_idle != idle)
		port->keepalives_idle = idle;
	if (port->keepalives_interval != interval)
		port->keepalives_interval = interval;
	return STATUS_OK;
}