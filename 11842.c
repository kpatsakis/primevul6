drv_nap(TERMINAL_CONTROL_BLOCK * TCB GCC_UNUSED, int ms)
{
#if HAVE_NANOSLEEP
    {
	struct timespec request, remaining;
	request.tv_sec = ms / 1000;
	request.tv_nsec = (ms % 1000) * 1000000;
	while (nanosleep(&request, &remaining) == -1
	       && errno == EINTR) {
	    request = remaining;
	}
    }
#else
    _nc_timed_wait(0, 0, ms, (int *) 0 EVENTLIST_2nd(0));
#endif
    return OK;
}