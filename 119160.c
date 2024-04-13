void cgtimer_time(cgtimer_t *ts_start)
{
	struct timeval tv;

	cgtime(&tv);
	ts_start->tv_sec = tv->tv_sec;
	ts_start->tv_nsec = tv->tv_usec * 1000;
}