void cgtimer_time(cgtimer_t *ts_start)
{
	clock_gettime(CLOCK_MONOTONIC, ts_start);
}