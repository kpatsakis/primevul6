	__must_hold(RCU)
{
	struct hlist_nulls_node *n;
	struct io_worker *worker;

	/*
	 * Iterate free_list and see if we can find an idle worker to
	 * activate. If a given worker is on the free_list but in the process
	 * of exiting, keep trying.
	 */
	hlist_nulls_for_each_entry_rcu(worker, n, &wqe->free_list, nulls_node) {
		if (!io_worker_get(worker))
			continue;
		if (io_wqe_get_acct(worker) != acct) {
			io_worker_release(worker);
			continue;
		}
		if (wake_up_process(worker->task)) {
			io_worker_release(worker);
			return true;
		}
		io_worker_release(worker);
	}

	return false;
}