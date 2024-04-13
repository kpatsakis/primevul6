socket_flush_if_writable(void)
{
	int			res;

	/* Quick exit if nothing to do */
	if (PqSendPointer == PqSendStart)
		return 0;

	/* No-op if reentrant call */
	if (PqCommBusy)
		return 0;

	/* Temporarily put the socket into non-blocking mode */
	socket_set_nonblocking(true);

	PqCommBusy = true;
	res = internal_flush();
	PqCommBusy = false;
	return res;
}