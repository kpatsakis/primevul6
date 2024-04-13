compress_init(const char *root)
{
	ds_ctxt_t		*ctxt;
	ds_compress_ctxt_t	*compress_ctxt;
	comp_thread_ctxt_t	*threads;

	/* Create and initialize the worker threads */
	threads = create_worker_threads(xtrabackup_compress_threads);
	if (threads == NULL) {
		msg("compress: failed to create worker threads.");
		return NULL;
	}

	ctxt = (ds_ctxt_t *) my_malloc(sizeof(ds_ctxt_t) +
				       sizeof(ds_compress_ctxt_t),
				       MYF(MY_FAE));

	compress_ctxt = (ds_compress_ctxt_t *) (ctxt + 1);
	compress_ctxt->threads = threads;
	compress_ctxt->nthreads = xtrabackup_compress_threads;

	ctxt->ptr = compress_ctxt;
	ctxt->root = my_strdup(root, MYF(MY_FAE));

	return ctxt;
}