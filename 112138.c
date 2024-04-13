pq_endmsgread(void)
{
	Assert(PqCommReadingMsg);

	PqCommReadingMsg = false;
}