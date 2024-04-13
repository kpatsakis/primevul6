pq_putmessage_v2(char msgtype, const char *s, size_t len)
{
	Assert(msgtype != 0);

	if (PqCommBusy)
		return 0;
	PqCommBusy = true;
	if (internal_putbytes(&msgtype, 1))
		goto fail;

	if (internal_putbytes(s, len))
		goto fail;
	PqCommBusy = false;
	return 0;

fail:
	PqCommBusy = false;
	return EOF;
}