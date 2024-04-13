pq_check_connection(void)
{
#if defined(POLLRDHUP)
	/*
	 * POLLRDHUP is a Linux extension to poll(2) to detect sockets closed by
	 * the other end.  We don't have a portable way to do that without
	 * actually trying to read or write data on other systems.  We don't want
	 * to read because that would be confused by pipelined queries and COPY
	 * data. Perhaps in future we'll try to write a heartbeat message instead.
	 */
	struct pollfd pollfd;
	int			rc;

	pollfd.fd = MyProcPort->sock;
	pollfd.events = POLLOUT | POLLIN | POLLRDHUP;
	pollfd.revents = 0;

	rc = poll(&pollfd, 1, 0);

	if (rc < 0)
	{
		ereport(COMMERROR,
				(errcode_for_socket_access(),
				 errmsg("could not poll socket: %m")));
		return false;
	}
	else if (rc == 1 && (pollfd.revents & (POLLHUP | POLLRDHUP)))
		return false;
#endif

	return true;
}