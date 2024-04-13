pq_startmsgread(void)
{
	/*
	 * There shouldn't be a read active already, but let's check just to be
	 * sure.
	 */
	if (PqCommReadingMsg)
		ereport(FATAL,
				(errcode(ERRCODE_PROTOCOL_VIOLATION),
				 errmsg("terminating connection because protocol synchronization was lost")));

	PqCommReadingMsg = true;
}