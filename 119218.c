void cgsem_destroy(cgsem_t *cgsem)
{
	sem_destroy(cgsem);
}