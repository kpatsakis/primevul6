CleanupBackend(int pid,
			   int exitstatus)	/* child's exit status. */
{
	dlist_mutable_iter iter;

	LogChildExit(DEBUG2, _("server process"), pid, exitstatus);

	/*
	 * If a backend dies in an ugly way then we must signal all other backends
	 * to quickdie.  If exit status is zero (normal) or one (FATAL exit), we
	 * assume everything is all right and proceed to remove the backend from
	 * the active backend list.
	 */

#ifdef WIN32

	/*
	 * On win32, also treat ERROR_WAIT_NO_CHILDREN (128) as nonfatal case,
	 * since that sometimes happens under load when the process fails to start
	 * properly (long before it starts using shared memory). Microsoft reports
	 * it is related to mutex failure:
	 * http://archives.postgresql.org/pgsql-hackers/2010-09/msg00790.php
	 */
	if (exitstatus == ERROR_WAIT_NO_CHILDREN)
	{
		LogChildExit(LOG, _("server process"), pid, exitstatus);
		exitstatus = 0;
	}
#endif

	if (!EXIT_STATUS_0(exitstatus) && !EXIT_STATUS_1(exitstatus))
	{
		HandleChildCrash(pid, exitstatus, _("server process"));
		return;
	}

	dlist_foreach_modify(iter, &BackendList)
	{
		Backend    *bp = dlist_container(Backend, elem, iter.cur);

		if (bp->pid == pid)
		{
			if (!bp->dead_end)
			{
				if (!ReleasePostmasterChildSlot(bp->child_slot))
				{
					/*
					 * Uh-oh, the child failed to clean itself up.  Treat as a
					 * crash after all.
					 */
					HandleChildCrash(pid, exitstatus, _("server process"));
					return;
				}
#ifdef EXEC_BACKEND
				ShmemBackendArrayRemove(bp);
#endif
			}
			if (bp->bgworker_notify)
			{
				/*
				 * This backend may have been slated to receive SIGUSR1 when
				 * some background worker started or stopped.  Cancel those
				 * notifications, as we don't want to signal PIDs that are not
				 * PostgreSQL backends.  This gets skipped in the (probably
				 * very common) case where the backend has never requested any
				 * such notifications.
				 */
				BackgroundWorkerStopNotifications(bp->pid);
			}
			dlist_delete(iter.cur);
			free(bp);
			break;
		}
	}
}