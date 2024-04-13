InitProcessGlobals(void)
{
	unsigned int rseed;

	MyProcPid = getpid();
	MyStartTimestamp = GetCurrentTimestamp();
	MyStartTime = timestamptz_to_time_t(MyStartTimestamp);

	/*
	 * Set a different seed for random() in every process.  We want something
	 * unpredictable, so if possible, use high-quality random bits for the
	 * seed.  Otherwise, fall back to a seed based on timestamp and PID.
	 */
	if (!pg_strong_random(&rseed, sizeof(rseed)))
	{
		/*
		 * Since PIDs and timestamps tend to change more frequently in their
		 * least significant bits, shift the timestamp left to allow a larger
		 * total number of seeds in a given time period.  Since that would
		 * leave only 20 bits of the timestamp that cycle every ~1 second,
		 * also mix in some higher bits.
		 */
		rseed = ((uint64) MyProcPid) ^
			((uint64) MyStartTimestamp << 12) ^
			((uint64) MyStartTimestamp >> 20);
	}
	srandom(rseed);
}