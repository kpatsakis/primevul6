CleanupBackgroundWorker(int pid,
						int exitstatus) /* child's exit status */
{
	char		namebuf[MAXPGPATH];
	slist_mutable_iter iter;

	slist_foreach_modify(iter, &BackgroundWorkerList)
	{
		RegisteredBgWorker *rw;

		rw = slist_container(RegisteredBgWorker, rw_lnode, iter.cur);

		if (rw->rw_pid != pid)
			continue;

#ifdef WIN32
		/* see CleanupBackend */
		if (exitstatus == ERROR_WAIT_NO_CHILDREN)
			exitstatus = 0;
#endif

		snprintf(namebuf, MAXPGPATH, _("background worker \"%s\""),
				 rw->rw_worker.bgw_type);


		if (!EXIT_STATUS_0(exitstatus))
		{
			/* Record timestamp, so we know when to restart the worker. */
			rw->rw_crashed_at = GetCurrentTimestamp();
		}
		else
		{
			/* Zero exit status means terminate */
			rw->rw_crashed_at = 0;
			rw->rw_terminate = true;
		}

		/*
		 * Additionally, just like a backend, any exit status other than 0 or
		 * 1 is considered a crash and causes a system-wide restart.
		 */
		if (!EXIT_STATUS_0(exitstatus) && !EXIT_STATUS_1(exitstatus))
		{
			HandleChildCrash(pid, exitstatus, namebuf);
			return true;
		}

		/*
		 * We must release the postmaster child slot. If the worker failed to
		 * do so, it did not clean up after itself, requiring a crash-restart
		 * cycle.
		 */
		if (!ReleasePostmasterChildSlot(rw->rw_child_slot))
		{
			HandleChildCrash(pid, exitstatus, namebuf);
			return true;
		}

		/* Get it out of the BackendList and clear out remaining data */
		dlist_delete(&rw->rw_backend->elem);
#ifdef EXEC_BACKEND
		ShmemBackendArrayRemove(rw->rw_backend);
#endif

		/*
		 * It's possible that this background worker started some OTHER
		 * background worker and asked to be notified when that worker started
		 * or stopped.  If so, cancel any notifications destined for the
		 * now-dead backend.
		 */
		if (rw->rw_backend->bgworker_notify)
			BackgroundWorkerStopNotifications(rw->rw_pid);
		free(rw->rw_backend);
		rw->rw_backend = NULL;
		rw->rw_pid = 0;
		rw->rw_child_slot = 0;
		ReportBackgroundWorkerExit(&iter);	/* report child death */

		LogChildExit(EXIT_STATUS_0(exitstatus) ? DEBUG1 : LOG,
					 namebuf, pid, exitstatus);

		return true;
	}

	return false;
}