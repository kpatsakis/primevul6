void _cgsem_post(cgsem_t *cgsem, const char *file, const char *func, const int line)
{
	const char buf = 1;
	int ret;

retry:
	ret = write(cgsem->pipefd[1], &buf, 1);
	if (unlikely(ret == 0))
		applog(LOG_WARNING, "Failed to write errno=%d" IN_FMT_FFL, errno, file, func, line);
	else if (unlikely(ret < 0 && interrupted))
		goto retry;
}