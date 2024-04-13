void cgsleep_ms_r(cgtimer_t *ts_start, int ms)
{
	LARGE_INTEGER li;

	li.QuadPart = ts_start->QuadPart + (int64_t)ms * 10000LL;
	liSleep(&li, ms);
}