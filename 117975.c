	__must_hold(wqe->lock)
{
	struct io_wq_work_node *node, *prev;
	struct io_wq_work *work, *tail;
	unsigned int stall_hash = -1U;
	struct io_wqe *wqe = worker->wqe;

	wq_list_for_each(node, prev, &acct->work_list) {
		unsigned int hash;

		work = container_of(node, struct io_wq_work, list);

		/* not hashed, can run anytime */
		if (!io_wq_is_hashed(work)) {
			wq_list_del(&acct->work_list, node, prev);
			return work;
		}

		hash = io_get_work_hash(work);
		/* all items with this hash lie in [work, tail] */
		tail = wqe->hash_tail[hash];

		/* hashed, can run if not already running */
		if (!test_and_set_bit(hash, &wqe->wq->hash->map)) {
			wqe->hash_tail[hash] = NULL;
			wq_list_cut(&acct->work_list, &tail->list, prev);
			return work;
		}
		if (stall_hash == -1U)
			stall_hash = hash;
		/* fast forward to a next hash, for-each will fix up @prev */
		node = &tail->list;
	}

	if (stall_hash != -1U) {
		/*
		 * Set this before dropping the lock to avoid racing with new
		 * work being added and clearing the stalled bit.
		 */
		set_bit(IO_ACCT_STALLED_BIT, &acct->flags);
		raw_spin_unlock(&wqe->lock);
		io_wait_on_hash(wqe, stall_hash);
		raw_spin_lock(&wqe->lock);
	}

	return NULL;
}