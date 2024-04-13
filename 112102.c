socket_comm_reset(void)
{
	/* Do not throw away pending data, but do reset the busy flag */
	PqCommBusy = false;
}