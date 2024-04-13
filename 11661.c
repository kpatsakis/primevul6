my_napms(int ms)
{
    if (ms > 0) {
#if defined(_WIN32) || !HAVE_GETTIMEOFDAY
	Sleep((DWORD) ms);
#else
	struct timeval data;
	data.tv_sec = 0;
	data.tv_usec = ms * 1000;
	select(0, NULL, NULL, NULL, &data);
#endif
    }
}