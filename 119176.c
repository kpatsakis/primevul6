void cgsleep_ms_r(cgtimer_t *ts_start, int ms)
{
	struct timespec ts_diff;

	ms_to_timespec(&ts_diff, ms);
	cgsleep_spec(&ts_diff, ts_start);
}