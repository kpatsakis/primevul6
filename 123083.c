static void tv_add(struct timeval *tv, int usec_delay)
{
	if (usec_delay > USEC) {
		tv->tv_sec += usec_delay / USEC;
		usec_delay %= USEC;
	}
	tv->tv_usec += usec_delay;

	if (tv->tv_usec > USEC) {
		tv->tv_sec += tv->tv_usec / USEC;
		tv->tv_usec %= USEC;
	}
}