sigusr1_handler(SIGNAL_ARGS)
{
	int			save_errno = errno;

	/*
	 * We rely on the signal mechanism to have blocked all signals ... except
	 * on Windows, which lacks sigaction(), so we have to do it manually.
	 */
#ifdef WIN32
	PG_SETMASK(&BlockSig);
#endif

	/*
	 * RECOVERY_STARTED and BEGIN_HOT_STANDBY signals are ignored in
	 * unexpected states. If the startup process quickly starts up, completes
	 * recovery, exits, we might process the death of the startup process
	 * first. We don't want to go back to recovery in that case.
	 */
	if (CheckPostmasterSignal(PMSIGNAL_RECOVERY_STARTED) &&
		pmState == PM_STARTUP && Shutdown == NoShutdown)
	{
		/* WAL redo has started. We're out of reinitialization. */
		FatalError = false;
		AbortStartTime = 0;

		/*
		 * Start the archiver if we're responsible for (re-)archiving received
		 * files.
		 */
		Assert(PgArchPID == 0);
		if (XLogArchivingAlways())
			PgArchPID = StartArchiver();

		/*
		 * If we aren't planning to enter hot standby mode later, treat
		 * RECOVERY_STARTED as meaning we're out of startup, and report status
		 * accordingly.
		 */
		if (!EnableHotStandby)
		{
			AddToDataDirLockFile(LOCK_FILE_LINE_PM_STATUS, PM_STATUS_STANDBY);
#ifdef USE_SYSTEMD
			sd_notify(0, "READY=1");
#endif
		}

		pmState = PM_RECOVERY;
	}

	if (CheckPostmasterSignal(PMSIGNAL_BEGIN_HOT_STANDBY) &&
		pmState == PM_RECOVERY && Shutdown == NoShutdown)
	{
		/*
		 * Likewise, start other special children as needed.
		 */
		Assert(PgStatPID == 0);
		PgStatPID = pgstat_start();

		ereport(LOG,
				(errmsg("database system is ready to accept read-only connections")));

		/* Report status */
		AddToDataDirLockFile(LOCK_FILE_LINE_PM_STATUS, PM_STATUS_READY);
#ifdef USE_SYSTEMD
		sd_notify(0, "READY=1");
#endif

		pmState = PM_HOT_STANDBY;
		connsAllowed = ALLOW_ALL_CONNS;

		/* Some workers may be scheduled to start now */
		StartWorkerNeeded = true;
	}

	/* Process background worker state changes. */
	if (CheckPostmasterSignal(PMSIGNAL_BACKGROUND_WORKER_CHANGE))
	{
		/* Accept new worker requests only if not stopping. */
		BackgroundWorkerStateChange(pmState < PM_STOP_BACKENDS);
		StartWorkerNeeded = true;
	}

	if (StartWorkerNeeded || HaveCrashedWorker)
		maybe_start_bgworkers();

	/* Tell syslogger to rotate logfile if requested */
	if (SysLoggerPID != 0)
	{
		if (CheckLogrotateSignal())
		{
			signal_child(SysLoggerPID, SIGUSR1);
			RemoveLogrotateSignalFiles();
		}
		else if (CheckPostmasterSignal(PMSIGNAL_ROTATE_LOGFILE))
		{
			signal_child(SysLoggerPID, SIGUSR1);
		}
	}

	if (CheckPostmasterSignal(PMSIGNAL_START_AUTOVAC_LAUNCHER) &&
		Shutdown <= SmartShutdown && pmState < PM_STOP_BACKENDS)
	{
		/*
		 * Start one iteration of the autovacuum daemon, even if autovacuuming
		 * is nominally not enabled.  This is so we can have an active defense
		 * against transaction ID wraparound.  We set a flag for the main loop
		 * to do it rather than trying to do it here --- this is because the
		 * autovac process itself may send the signal, and we want to handle
		 * that by launching another iteration as soon as the current one
		 * completes.
		 */
		start_autovac_launcher = true;
	}

	if (CheckPostmasterSignal(PMSIGNAL_START_AUTOVAC_WORKER) &&
		Shutdown <= SmartShutdown && pmState < PM_STOP_BACKENDS)
	{
		/* The autovacuum launcher wants us to start a worker process. */
		StartAutovacuumWorker();
	}

	if (CheckPostmasterSignal(PMSIGNAL_START_WALRECEIVER))
	{
		/* Startup Process wants us to start the walreceiver process. */
		/* Start immediately if possible, else remember request for later. */
		WalReceiverRequested = true;
		MaybeStartWalReceiver();
	}

	/*
	 * Try to advance postmaster's state machine, if a child requests it.
	 *
	 * Be careful about the order of this action relative to sigusr1_handler's
	 * other actions.  Generally, this should be after other actions, in case
	 * they have effects PostmasterStateMachine would need to know about.
	 * However, we should do it before the CheckPromoteSignal step, which
	 * cannot have any (immediate) effect on the state machine, but does
	 * depend on what state we're in now.
	 */
	if (CheckPostmasterSignal(PMSIGNAL_ADVANCE_STATE_MACHINE))
	{
		PostmasterStateMachine();
	}

	if (StartupPID != 0 &&
		(pmState == PM_STARTUP || pmState == PM_RECOVERY ||
		 pmState == PM_HOT_STANDBY) &&
		CheckPromoteSignal())
	{
		/*
		 * Tell startup process to finish recovery.
		 *
		 * Leave the promote signal file in place and let the Startup process
		 * do the unlink.
		 */
		signal_child(StartupPID, SIGUSR2);
	}

#ifdef WIN32
	PG_SETMASK(&UnBlockSig);
#endif

	errno = save_errno;
}