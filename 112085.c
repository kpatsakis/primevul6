HandleChildCrash(int pid, int exitstatus, const char *procname)
{
	dlist_mutable_iter iter;
	slist_iter	siter;
	Backend    *bp;
	bool		take_action;

	/*
	 * We only log messages and send signals if this is the first process
	 * crash and we're not doing an immediate shutdown; otherwise, we're only
	 * here to update postmaster's idea of live processes.  If we have already
	 * signaled children, nonzero exit status is to be expected, so don't
	 * clutter log.
	 */
	take_action = !FatalError && Shutdown != ImmediateShutdown;

	if (take_action)
	{
		LogChildExit(LOG, procname, pid, exitstatus);
		ereport(LOG,
				(errmsg("terminating any other active server processes")));
		SetQuitSignalReason(PMQUIT_FOR_CRASH);
	}

	/* Process background workers. */
	slist_foreach(siter, &BackgroundWorkerList)
	{
		RegisteredBgWorker *rw;

		rw = slist_container(RegisteredBgWorker, rw_lnode, siter.cur);
		if (rw->rw_pid == 0)
			continue;			/* not running */
		if (rw->rw_pid == pid)
		{
			/*
			 * Found entry for freshly-dead worker, so remove it.
			 */
			(void) ReleasePostmasterChildSlot(rw->rw_child_slot);
			dlist_delete(&rw->rw_backend->elem);
#ifdef EXEC_BACKEND
			ShmemBackendArrayRemove(rw->rw_backend);
#endif
			free(rw->rw_backend);
			rw->rw_backend = NULL;
			rw->rw_pid = 0;
			rw->rw_child_slot = 0;
			/* don't reset crashed_at */
			/* don't report child stop, either */
			/* Keep looping so we can signal remaining workers */
		}
		else
		{
			/*
			 * This worker is still alive.  Unless we did so already, tell it
			 * to commit hara-kiri.
			 *
			 * SIGQUIT is the special signal that says exit without proc_exit
			 * and let the user know what's going on. But if SendStop is set
			 * (-T on command line), then we send SIGSTOP instead, so that we
			 * can get core dumps from all backends by hand.
			 */
			if (take_action)
			{
				ereport(DEBUG2,
						(errmsg_internal("sending %s to process %d",
										 (SendStop ? "SIGSTOP" : "SIGQUIT"),
										 (int) rw->rw_pid)));
				signal_child(rw->rw_pid, (SendStop ? SIGSTOP : SIGQUIT));
			}
		}
	}

	/* Process regular backends */
	dlist_foreach_modify(iter, &BackendList)
	{
		bp = dlist_container(Backend, elem, iter.cur);

		if (bp->pid == pid)
		{
			/*
			 * Found entry for freshly-dead backend, so remove it.
			 */
			if (!bp->dead_end)
			{
				(void) ReleasePostmasterChildSlot(bp->child_slot);
#ifdef EXEC_BACKEND
				ShmemBackendArrayRemove(bp);
#endif
			}
			dlist_delete(iter.cur);
			free(bp);
			/* Keep looping so we can signal remaining backends */
		}
		else
		{
			/*
			 * This backend is still alive.  Unless we did so already, tell it
			 * to commit hara-kiri.
			 *
			 * SIGQUIT is the special signal that says exit without proc_exit
			 * and let the user know what's going on. But if SendStop is set
			 * (-T on command line), then we send SIGSTOP instead, so that we
			 * can get core dumps from all backends by hand.
			 *
			 * We could exclude dead_end children here, but at least in the
			 * SIGSTOP case it seems better to include them.
			 *
			 * Background workers were already processed above; ignore them
			 * here.
			 */
			if (bp->bkend_type == BACKEND_TYPE_BGWORKER)
				continue;

			if (take_action)
			{
				ereport(DEBUG2,
						(errmsg_internal("sending %s to process %d",
										 (SendStop ? "SIGSTOP" : "SIGQUIT"),
										 (int) bp->pid)));
				signal_child(bp->pid, (SendStop ? SIGSTOP : SIGQUIT));
			}
		}
	}

	/* Take care of the startup process too */
	if (pid == StartupPID)
	{
		StartupPID = 0;
		/* Caller adjusts StartupStatus, so don't touch it here */
	}
	else if (StartupPID != 0 && take_action)
	{
		ereport(DEBUG2,
				(errmsg_internal("sending %s to process %d",
								 (SendStop ? "SIGSTOP" : "SIGQUIT"),
								 (int) StartupPID)));
		signal_child(StartupPID, (SendStop ? SIGSTOP : SIGQUIT));
		StartupStatus = STARTUP_SIGNALED;
	}

	/* Take care of the bgwriter too */
	if (pid == BgWriterPID)
		BgWriterPID = 0;
	else if (BgWriterPID != 0 && take_action)
	{
		ereport(DEBUG2,
				(errmsg_internal("sending %s to process %d",
								 (SendStop ? "SIGSTOP" : "SIGQUIT"),
								 (int) BgWriterPID)));
		signal_child(BgWriterPID, (SendStop ? SIGSTOP : SIGQUIT));
	}

	/* Take care of the checkpointer too */
	if (pid == CheckpointerPID)
		CheckpointerPID = 0;
	else if (CheckpointerPID != 0 && take_action)
	{
		ereport(DEBUG2,
				(errmsg_internal("sending %s to process %d",
								 (SendStop ? "SIGSTOP" : "SIGQUIT"),
								 (int) CheckpointerPID)));
		signal_child(CheckpointerPID, (SendStop ? SIGSTOP : SIGQUIT));
	}

	/* Take care of the walwriter too */
	if (pid == WalWriterPID)
		WalWriterPID = 0;
	else if (WalWriterPID != 0 && take_action)
	{
		ereport(DEBUG2,
				(errmsg_internal("sending %s to process %d",
								 (SendStop ? "SIGSTOP" : "SIGQUIT"),
								 (int) WalWriterPID)));
		signal_child(WalWriterPID, (SendStop ? SIGSTOP : SIGQUIT));
	}

	/* Take care of the walreceiver too */
	if (pid == WalReceiverPID)
		WalReceiverPID = 0;
	else if (WalReceiverPID != 0 && take_action)
	{
		ereport(DEBUG2,
				(errmsg_internal("sending %s to process %d",
								 (SendStop ? "SIGSTOP" : "SIGQUIT"),
								 (int) WalReceiverPID)));
		signal_child(WalReceiverPID, (SendStop ? SIGSTOP : SIGQUIT));
	}

	/* Take care of the autovacuum launcher too */
	if (pid == AutoVacPID)
		AutoVacPID = 0;
	else if (AutoVacPID != 0 && take_action)
	{
		ereport(DEBUG2,
				(errmsg_internal("sending %s to process %d",
								 (SendStop ? "SIGSTOP" : "SIGQUIT"),
								 (int) AutoVacPID)));
		signal_child(AutoVacPID, (SendStop ? SIGSTOP : SIGQUIT));
	}

	/* Take care of the archiver too */
	if (pid == PgArchPID)
		PgArchPID = 0;
	else if (PgArchPID != 0 && take_action)
	{
		ereport(DEBUG2,
				(errmsg_internal("sending %s to process %d",
								 (SendStop ? "SIGSTOP" : "SIGQUIT"),
								 (int) PgArchPID)));
		signal_child(PgArchPID, (SendStop ? SIGSTOP : SIGQUIT));
	}

	/*
	 * Force a power-cycle of the pgstat process too.  (This isn't absolutely
	 * necessary, but it seems like a good idea for robustness, and it
	 * simplifies the state-machine logic in the case where a shutdown request
	 * arrives during crash processing.)
	 */
	if (PgStatPID != 0 && take_action)
	{
		ereport(DEBUG2,
				(errmsg_internal("sending %s to process %d",
								 "SIGQUIT",
								 (int) PgStatPID)));
		signal_child(PgStatPID, SIGQUIT);
		allow_immediate_pgstat_restart();
	}

	/* We do NOT restart the syslogger */

	if (Shutdown != ImmediateShutdown)
		FatalError = true;

	/* We now transit into a state of waiting for children to die */
	if (pmState == PM_RECOVERY ||
		pmState == PM_HOT_STANDBY ||
		pmState == PM_RUN ||
		pmState == PM_STOP_BACKENDS ||
		pmState == PM_SHUTDOWN)
		pmState = PM_WAIT_BACKENDS;

	/*
	 * .. and if this doesn't happen quickly enough, now the clock is ticking
	 * for us to kill them without mercy.
	 */
	if (AbortStartTime == 0)
		AbortStartTime = time(NULL);
}