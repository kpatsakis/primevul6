void cgsleep_us_r(cgtimer_t *ts_start, int64_t us)
{
	struct timespec ts_end;

	us_to_timespec(&ts_end, us);
	timeraddspec(&ts_end, ts_start);
	nanosleep_abstime(&ts_end);
}