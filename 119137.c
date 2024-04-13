void _cgsem_wait(cgsem_t *cgsem, const char *file, const char *func, const int line)
{
retry:
	if (unlikely(sem_wait(cgsem))) {
		if (interrupted())
			goto retry;
		quitfrom(1, file, func, line, "Failed to sem_wait errno=%d cgsem=0x%p", errno, cgsem);
	}
}