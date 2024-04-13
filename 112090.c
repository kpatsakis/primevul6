socket_putmessage_noblock(char msgtype, const char *s, size_t len)
{
	int			res PG_USED_FOR_ASSERTS_ONLY;
	int			required;

	/*
	 * Ensure we have enough space in the output buffer for the message header
	 * as well as the message itself.
	 */
	required = PqSendPointer + 1 + 4 + len;
	if (required > PqSendBufferSize)
	{
		PqSendBuffer = repalloc(PqSendBuffer, required);
		PqSendBufferSize = required;
	}
	res = pq_putmessage(msgtype, s, len);
	Assert(res == 0);			/* should not fail when the message fits in
								 * buffer */
}