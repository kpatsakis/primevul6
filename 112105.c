socket_putmessage(char msgtype, const char *s, size_t len)
{
	uint32		n32;

	Assert(msgtype != 0);

	if (PqCommBusy)
		return 0;
	PqCommBusy = true;
	if (internal_putbytes(&msgtype, 1))
		goto fail;

	n32 = pg_hton32((uint32) (len + 4));
	if (internal_putbytes((char *) &n32, 4))
		goto fail;

	if (internal_putbytes(s, len))
		goto fail;
	PqCommBusy = false;
	return 0;

fail:
	PqCommBusy = false;
	return EOF;
}