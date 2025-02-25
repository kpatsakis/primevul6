int _cgsem_mswait(cgsem_t *cgsem, int ms, const char *file, const char *func, const int line)
{
	struct timespec abs_timeout, ts_now;
	struct timeval tv_now;
	int ret;

	cgtime(&tv_now);
	timeval_to_spec(&ts_now, &tv_now);
	ms_to_timespec(&abs_timeout, ms);
retry:
	timeraddspec(&abs_timeout, &ts_now);
	ret = sem_timedwait(cgsem, &abs_timeout);

	if (ret) {
		if (likely(sock_timeout()))
			return ETIMEDOUT;
		if (interrupted())
			goto retry;
		quitfrom(1, file, func, line, "Failed to sem_timedwait errno=%d cgsem=0x%p", errno, cgsem);
	}
	return 0;
}