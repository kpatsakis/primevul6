	__releases(wqe->lock)
{
	struct io_wq_work_node *node, *prev;
	struct io_wq_work *work;

	wq_list_for_each(node, prev, &acct->work_list) {
		work = container_of(node, struct io_wq_work, list);
		if (!match->fn(work, match->data))
			continue;
		io_wqe_remove_pending(wqe, work, prev);
		raw_spin_unlock(&wqe->lock);
		io_run_cancel(work, wqe);
		match->nr_pending++;
		/* not safe to continue after unlock */
		return true;
	}

	return false;
}