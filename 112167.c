internal_putbytes(const char *s, size_t len)
{
	size_t		amount;

	while (len > 0)
	{
		/* If buffer is full, then flush it out */
		if (PqSendPointer >= PqSendBufferSize)
		{
			socket_set_nonblocking(false);
			if (internal_flush())
				return EOF;
		}
		amount = PqSendBufferSize - PqSendPointer;
		if (amount > len)
			amount = len;
		memcpy(PqSendBuffer + PqSendPointer, s, amount);
		PqSendPointer += amount;
		s += amount;
		len -= amount;
	}
	return 0;
}