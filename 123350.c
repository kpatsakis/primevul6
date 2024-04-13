create_worker_threads(uint n)
{
	comp_thread_ctxt_t	*threads;
	uint 			i;

	threads = (comp_thread_ctxt_t *)
		my_malloc(sizeof(comp_thread_ctxt_t) * n, MYF(MY_FAE));

	for (i = 0; i < n; i++) {
		comp_thread_ctxt_t *thd = threads + i;

		thd->num = i + 1;
		thd->cancelled = FALSE;
		thd->data_avail = FALSE;

		thd->to = (char *) my_malloc(COMPRESS_CHUNK_SIZE +
						   MY_QLZ_COMPRESS_OVERHEAD,
						   MYF(MY_FAE));

		/* Initialize and data mutex and condition var */
		if (pthread_mutex_init(&thd->data_mutex, NULL) ||
		    pthread_cond_init(&thd->data_cond, NULL)) {
			goto err;
		}

		if (pthread_create(&thd->id, NULL, compress_worker_thread_func,
				   thd)) {
			msg("compress: pthread_create() failed: "
			    "errno = %d", errno);
			goto err;
		}
	}

	return threads;

err:
	for (; i; i--) {
		destroy_worker_thread(threads + i);
	}

	my_free(threads);
	return NULL;
}