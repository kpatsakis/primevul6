	__must_hold(wqe->lock)
{
	if (worker->flags & IO_WORKER_F_FREE) {
		worker->flags &= ~IO_WORKER_F_FREE;
		hlist_nulls_del_init_rcu(&worker->nulls_node);
	}
}