void cgtimer_time(cgtimer_t *ts_start)
{
	FILETIME ft;

	GetSystemTimeAsFileTime(&ft);
	ts_start->LowPart = ft.dwLowDateTime;
	ts_start->HighPart = ft.dwHighDateTime;
}