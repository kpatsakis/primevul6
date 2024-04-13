destroy_worker_threads(comp_thread_ctxt_t *threads, uint n)
{
	uint i;

	for (i = 0; i < n; i++) {
		destroy_worker_thread(threads + i);
	}

	my_free(threads);
}