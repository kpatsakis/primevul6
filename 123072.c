void radius_signal_self(int flag)
{
	ssize_t rcode;
	uint8_t buffer[16];

	/*
	 *	The read MUST be non-blocking for this to work.
	 */
	rcode = read(self_pipe[0], buffer, sizeof(buffer));
	if (rcode > 0) {
		ssize_t i;

		for (i = 0; i < rcode; i++) {
			buffer[0] |= buffer[i];
		}
	} else {
		buffer[0] = 0;
	}

	buffer[0] |= flag;

	write(self_pipe[1], buffer, 1);
}