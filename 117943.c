static bool io_worker_get(struct io_worker *worker)
{
	return refcount_inc_not_zero(&worker->ref);
}