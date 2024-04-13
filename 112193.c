socket_flush(void)
{
	int			res;

	/* No-op if reentrant call */
	if (PqCommBusy)
		return 0;
	PqCommBusy = true;
	socket_set_nonblocking(false);
	res = internal_flush();
	PqCommBusy = false;
	return res;
}