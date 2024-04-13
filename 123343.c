destroy_worker_thread(comp_thread_ctxt_t *thd)
{
	pthread_mutex_lock(&thd->data_mutex);
	thd->cancelled = TRUE;
	pthread_cond_signal(&thd->data_cond);
	pthread_mutex_unlock(&thd->data_mutex);

	pthread_join(thd->id, NULL);

	pthread_cond_destroy(&thd->data_cond);
	pthread_mutex_destroy(&thd->data_mutex);

	my_free(thd->to);
}