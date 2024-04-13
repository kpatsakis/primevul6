void nmsleep(unsigned int msecs)
{
	struct timespec twait, tleft;
	int ret;
	ldiv_t d;

	d = ldiv(msecs, 1000);
	tleft.tv_sec = d.quot;
	tleft.tv_nsec = d.rem * 1000000;
	do {
		twait.tv_sec = tleft.tv_sec;
		twait.tv_nsec = tleft.tv_nsec;
		ret = nanosleep(&twait, &tleft);
	} while (ret == -1 && errno == EINTR);
}