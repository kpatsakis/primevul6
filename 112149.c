pmdie(SIGNAL_ARGS)
{
	int			save_errno = errno;

	/*
	 * We rely on the signal mechanism to have blocked all signals ... except
	 * on Windows, which lacks sigaction(), so we have to do it manually.
	 */
#ifdef WIN32
	PG_SETMASK(&BlockSig);
#endif

	ereport(DEBUG2,
			(errmsg_internal("postmaster received signal %d",
							 postgres_signal_arg)));

	switch (postgres_signal_arg)
	{
		case SIGTERM:

			/*
			 * Smart Shutdown:
			 *
			 * Wait for children to end their work, then shut down.
			 */
			if (Shutdown >= SmartShutdown)
				break;
			Shutdown = SmartShutdown;
			ereport(LOG,
					(errmsg("received smart shutdown request")));

			/* Report status */
			AddToDataDirLockFile(LOCK_FILE_LINE_PM_STATUS, PM_STATUS_STOPPING);
#ifdef USE_SYSTEMD
			sd_notify(0, "STOPPING=1");
#endif

			/*
			 * If we reached normal running, we have to wait for any online
			 * backup mode to end; otherwise go straight to waiting for client
			 * backends to exit.  (The difference is that in the former state,
			 * we'll still let in new superuser clients, so that somebody can
			 * end the online backup mode.)  If already in PM_STOP_BACKENDS or
			 * a later state, do not change it.
			 */
			if (pmState == PM_RUN)
				connsAllowed = ALLOW_SUPERUSER_CONNS;
			else if (pmState == PM_HOT_STANDBY)
				connsAllowed = ALLOW_NO_CONNS;
			else if (pmState == PM_STARTUP || pmState == PM_RECOVERY)
			{
				/* There should be no clients, so proceed to stop children */
				pmState = PM_STOP_BACKENDS;
			}

			/*
			 * Now wait for online backup mode to end and backends to exit. If
			 * that is already the case, PostmasterStateMachine will take the
			 * next step.
			 */
			PostmasterStateMachine();
			break;

		case SIGINT:

			/*
			 * Fast Shutdown:
			 *
			 * Abort all children with SIGTERM (rollback active transactions
			 * and exit) and shut down when they are gone.
			 */
			if (Shutdown >= FastShutdown)
				break;
			Shutdown = FastShutdown;
			ereport(LOG,
					(errmsg("received fast shutdown request")));

			/* Report status */
			AddToDataDirLockFile(LOCK_FILE_LINE_PM_STATUS, PM_STATUS_STOPPING);
#ifdef USE_SYSTEMD
			sd_notify(0, "STOPPING=1");
#endif

			if (pmState == PM_STARTUP || pmState == PM_RECOVERY)
			{
				/* Just shut down background processes silently */
				pmState = PM_STOP_BACKENDS;
			}
			else if (pmState == PM_RUN ||
					 pmState == PM_HOT_STANDBY)
			{
				/* Report that we're about to zap live client sessions */
				ereport(LOG,
						(errmsg("aborting any active transactions")));
				pmState = PM_STOP_BACKENDS;
			}

			/*
			 * PostmasterStateMachine will issue any necessary signals, or
			 * take the next step if no child processes need to be killed.
			 */
			PostmasterStateMachine();
			break;

		case SIGQUIT:

			/*
			 * Immediate Shutdown:
			 *
			 * abort all children with SIGQUIT, wait for them to exit,
			 * terminate remaining ones with SIGKILL, then exit without
			 * attempt to properly shut down the data base system.
			 */
			if (Shutdown >= ImmediateShutdown)
				break;
			Shutdown = ImmediateShutdown;
			ereport(LOG,
					(errmsg("received immediate shutdown request")));

			/* Report status */
			AddToDataDirLockFile(LOCK_FILE_LINE_PM_STATUS, PM_STATUS_STOPPING);
#ifdef USE_SYSTEMD
			sd_notify(0, "STOPPING=1");
#endif

			/* tell children to shut down ASAP */
			SetQuitSignalReason(PMQUIT_FOR_STOP);
			TerminateChildren(SIGQUIT);
			pmState = PM_WAIT_BACKENDS;

			/* set stopwatch for them to die */
			AbortStartTime = time(NULL);

			/*
			 * Now wait for backends to exit.  If there are none,
			 * PostmasterStateMachine will take the next step.
			 */
			PostmasterStateMachine();
			break;
	}

#ifdef WIN32
	PG_SETMASK(&UnBlockSig);
#endif

	errno = save_errno;
}