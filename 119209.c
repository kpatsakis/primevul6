static void cgsleep_spec(struct timespec *ts_diff, const struct timespec *ts_start)
{
	struct timespec now;

	timeraddspec(ts_diff, ts_start);
	cgtimer_time(&now);
	timersubspec(ts_diff, &now);
	if (unlikely(ts_diff->tv_sec < 0))
		return;
	nanosleep(ts_diff, NULL);
}