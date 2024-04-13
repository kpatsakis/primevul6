void cgsem_reset(cgsem_t *cgsem)
{
	int ret;

	do {
		ret = sem_trywait(cgsem);
		if (unlikely(ret < 0 && interrupted()))
			ret = 0;
	} while (!ret);
}