int _cgsem_mswait(cgsem_t *cgsem, int ms, const char *file, const char *func, const int line)
{
	struct timeval timeout;
	int ret, fd;
	fd_set rd;
	char buf;

retry:
	fd = cgsem->pipefd[0];
	FD_ZERO(&rd);
	FD_SET(fd, &rd);
	ms_to_timeval(&timeout, ms);
	ret = select(fd + 1, &rd, NULL, NULL, &timeout);

	if (ret > 0) {
		ret = read(fd, &buf, 1);
		return 0;
	}
	if (likely(!ret))
		return ETIMEDOUT;
	if (interrupted())
		goto retry;
	quitfrom(1, file, func, line, "Failed to sem_timedwait errno=%d cgsem=0x%p", errno, cgsem);
	/* We don't reach here */
	return 0;
}