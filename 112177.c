StartChildProcess(AuxProcType type)
{
	pid_t		pid;

#ifdef EXEC_BACKEND
	{
		char	   *av[10];
		int			ac = 0;
		char		typebuf[32];

		/*
		 * Set up command-line arguments for subprocess
		 */
		av[ac++] = "postgres";
		av[ac++] = "--forkaux";
		av[ac++] = NULL;		/* filled in by postmaster_forkexec */

		snprintf(typebuf, sizeof(typebuf), "%d", type);
		av[ac++] = typebuf;

		av[ac] = NULL;
		Assert(ac < lengthof(av));

		pid = postmaster_forkexec(ac, av);
	}
#else							/* !EXEC_BACKEND */
	pid = fork_process();

	if (pid == 0)				/* child */
	{
		InitPostmasterChild();

		/* Close the postmaster's sockets */
		ClosePostmasterPorts(false);

		/* Release postmaster's working memory context */
		MemoryContextSwitchTo(TopMemoryContext);
		MemoryContextDelete(PostmasterContext);
		PostmasterContext = NULL;

		AuxiliaryProcessMain(type); /* does not return */
	}
#endif							/* EXEC_BACKEND */

	if (pid < 0)
	{
		/* in parent, fork failed */
		int			save_errno = errno;

		errno = save_errno;
		switch (type)
		{
			case StartupProcess:
				ereport(LOG,
						(errmsg("could not fork startup process: %m")));
				break;
			case ArchiverProcess:
				ereport(LOG,
						(errmsg("could not fork archiver process: %m")));
				break;
			case BgWriterProcess:
				ereport(LOG,
						(errmsg("could not fork background writer process: %m")));
				break;
			case CheckpointerProcess:
				ereport(LOG,
						(errmsg("could not fork checkpointer process: %m")));
				break;
			case WalWriterProcess:
				ereport(LOG,
						(errmsg("could not fork WAL writer process: %m")));
				break;
			case WalReceiverProcess:
				ereport(LOG,
						(errmsg("could not fork WAL receiver process: %m")));
				break;
			default:
				ereport(LOG,
						(errmsg("could not fork process: %m")));
				break;
		}

		/*
		 * fork failure is fatal during startup, but there's no need to choke
		 * immediately if starting other child types fails.
		 */
		if (type == StartupProcess)
			ExitPostmaster(1);
		return 0;
	}

	/*
	 * in parent, successful fork
	 */
	return pid;
}