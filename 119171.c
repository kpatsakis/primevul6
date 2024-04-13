void ms_to_timespec(struct timespec *spec, int64_t ms)
{
	lldiv_t tvdiv = lldiv(ms, 1000);

	spec->tv_sec = tvdiv.quot;
	spec->tv_nsec = tvdiv.rem * 1000000;
}