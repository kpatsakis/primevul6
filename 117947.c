	__must_hold(wqe->lock)
{
	struct io_wqe_acct *acct = io_wqe_get_acct(worker);
	struct io_wqe *wqe = worker->wqe;

	if (!(worker->flags & IO_WORKER_F_UP))
		return;

	if (atomic_dec_and_test(&acct->nr_running) && io_acct_run_queue(acct)) {
		atomic_inc(&acct->nr_running);
		atomic_inc(&wqe->wq->worker_refs);
		io_queue_worker_create(worker, acct, create_worker_cb);
	}
}