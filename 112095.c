SIGHUP_handler(SIGNAL_ARGS)
{
	int			save_errno = errno;

	/*
	 * We rely on the signal mechanism to have blocked all signals ... except
	 * on Windows, which lacks sigaction(), so we have to do it manually.
	 */
#ifdef WIN32
	PG_SETMASK(&BlockSig);
#endif

	if (Shutdown <= SmartShutdown)
	{
		ereport(LOG,
				(errmsg("received SIGHUP, reloading configuration files")));
		ProcessConfigFile(PGC_SIGHUP);
		SignalChildren(SIGHUP);
		if (StartupPID != 0)
			signal_child(StartupPID, SIGHUP);
		if (BgWriterPID != 0)
			signal_child(BgWriterPID, SIGHUP);
		if (CheckpointerPID != 0)
			signal_child(CheckpointerPID, SIGHUP);
		if (WalWriterPID != 0)
			signal_child(WalWriterPID, SIGHUP);
		if (WalReceiverPID != 0)
			signal_child(WalReceiverPID, SIGHUP);
		if (AutoVacPID != 0)
			signal_child(AutoVacPID, SIGHUP);
		if (PgArchPID != 0)
			signal_child(PgArchPID, SIGHUP);
		if (SysLoggerPID != 0)
			signal_child(SysLoggerPID, SIGHUP);
		if (PgStatPID != 0)
			signal_child(PgStatPID, SIGHUP);

		/* Reload authentication config files too */
		if (!load_hba())
			ereport(LOG,
			/* translator: %s is a configuration file */
					(errmsg("%s was not reloaded", "pg_hba.conf")));

		if (!load_ident())
			ereport(LOG,
					(errmsg("%s was not reloaded", "pg_ident.conf")));

#ifdef USE_SSL
		/* Reload SSL configuration as well */
		if (EnableSSL)
		{
			if (secure_initialize(false) == 0)
				LoadedSSL = true;
			else
				ereport(LOG,
						(errmsg("SSL configuration was not reloaded")));
		}
		else
		{
			secure_destroy();
			LoadedSSL = false;
		}
#endif

#ifdef EXEC_BACKEND
		/* Update the starting-point file for future children */
		write_nondefault_variables(PGC_SIGHUP);
#endif
	}

#ifdef WIN32
	PG_SETMASK(&UnBlockSig);
#endif

	errno = save_errno;
}