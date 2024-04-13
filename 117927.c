	__releases(wqe->lock)
{
	struct io_wqe_acct *acct = io_wqe_get_acct(worker);
	struct io_wqe *wqe = worker->wqe;
	struct io_wq *wq = wqe->wq;
	bool do_kill = test_bit(IO_WQ_BIT_EXIT, &wq->state);

	do {
		struct io_wq_work *work;
get_next:
		/*
		 * If we got some work, mark us as busy. If we didn't, but
		 * the list isn't empty, it means we stalled on hashed work.
		 * Mark us stalled so we don't keep looking for work when we
		 * can't make progress, any work completion or insertion will
		 * clear the stalled flag.
		 */
		work = io_get_next_work(acct, worker);
		if (work)
			__io_worker_busy(wqe, worker, work);

		raw_spin_unlock(&wqe->lock);
		if (!work)
			break;
		io_assign_current_work(worker, work);
		__set_current_state(TASK_RUNNING);

		/* handle a whole dependent link */
		do {
			struct io_wq_work *next_hashed, *linked;
			unsigned int hash = io_get_work_hash(work);

			next_hashed = wq_next_work(work);

			if (unlikely(do_kill) && (work->flags & IO_WQ_WORK_UNBOUND))
				work->flags |= IO_WQ_WORK_CANCEL;
			wq->do_work(work);
			io_assign_current_work(worker, NULL);

			linked = wq->free_work(work);
			work = next_hashed;
			if (!work && linked && !io_wq_is_hashed(linked)) {
				work = linked;
				linked = NULL;
			}
			io_assign_current_work(worker, work);
			if (linked)
				io_wqe_enqueue(wqe, linked);

			if (hash != -1U && !next_hashed) {
				clear_bit(hash, &wq->hash->map);
				clear_bit(IO_ACCT_STALLED_BIT, &acct->flags);
				if (wq_has_sleeper(&wq->hash->wait))
					wake_up(&wq->hash->wait);
				raw_spin_lock(&wqe->lock);
				/* skip unnecessary unlock-lock wqe->lock */
				if (!work)
					goto get_next;
				raw_spin_unlock(&wqe->lock);
			}
		} while (work);

		raw_spin_lock(&wqe->lock);
	} while (1);
}