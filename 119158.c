int thr_info_create(struct thr_info *thr, pthread_attr_t *attr, void *(*start) (void *), void *arg)
{
	cgsem_init(&thr->sem);

	return pthread_create(&thr->pth, attr, start, arg);
}