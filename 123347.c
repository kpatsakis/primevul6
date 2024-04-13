compress_deinit(ds_ctxt_t *ctxt)
{
	ds_compress_ctxt_t 	*comp_ctxt;

	xb_ad(ctxt->pipe_ctxt != NULL);

	comp_ctxt = (ds_compress_ctxt_t *) ctxt->ptr;;

	destroy_worker_threads(comp_ctxt->threads, comp_ctxt->nthreads);

	my_free(ctxt->root);
	my_free(ctxt);
}