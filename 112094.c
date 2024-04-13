pq_recvbuf(void)
{
	if (PqRecvPointer > 0)
	{
		if (PqRecvLength > PqRecvPointer)
		{
			/* still some unread data, left-justify it in the buffer */
			memmove(PqRecvBuffer, PqRecvBuffer + PqRecvPointer,
					PqRecvLength - PqRecvPointer);
			PqRecvLength -= PqRecvPointer;
			PqRecvPointer = 0;
		}
		else
			PqRecvLength = PqRecvPointer = 0;
	}

	/* Ensure that we're in blocking mode */
	socket_set_nonblocking(false);

	/* Can fill buffer from PqRecvLength and upwards */
	for (;;)
	{
		int			r;

		r = secure_read(MyProcPort, PqRecvBuffer + PqRecvLength,
						PQ_RECV_BUFFER_SIZE - PqRecvLength);

		if (r < 0)
		{
			if (errno == EINTR)
				continue;		/* Ok if interrupted */

			/*
			 * Careful: an ereport() that tries to write to the client would
			 * cause recursion to here, leading to stack overflow and core
			 * dump!  This message must go *only* to the postmaster log.
			 */
			ereport(COMMERROR,
					(errcode_for_socket_access(),
					 errmsg("could not receive data from client: %m")));
			return EOF;
		}
		if (r == 0)
		{
			/*
			 * EOF detected.  We used to write a log message here, but it's
			 * better to expect the ultimate caller to do that.
			 */
			return EOF;
		}
		/* r contains number of bytes read, so just incr length */
		PqRecvLength += r;
		return 0;
	}
}