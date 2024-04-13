SignalSomeChildren(int signal, int target)
{
	dlist_iter	iter;
	bool		signaled = false;

	dlist_foreach(iter, &BackendList)
	{
		Backend    *bp = dlist_container(Backend, elem, iter.cur);

		if (bp->dead_end)
			continue;

		/*
		 * Since target == BACKEND_TYPE_ALL is the most common case, we test
		 * it first and avoid touching shared memory for every child.
		 */
		if (target != BACKEND_TYPE_ALL)
		{
			/*
			 * Assign bkend_type for any recently announced WAL Sender
			 * processes.
			 */
			if (bp->bkend_type == BACKEND_TYPE_NORMAL &&
				IsPostmasterChildWalSender(bp->child_slot))
				bp->bkend_type = BACKEND_TYPE_WALSND;

			if (!(target & bp->bkend_type))
				continue;
		}

		ereport(DEBUG4,
				(errmsg_internal("sending signal %d to process %d",
								 signal, (int) bp->pid)));
		signal_child(bp->pid, signal);
		signaled = true;
	}
	return signaled;
}