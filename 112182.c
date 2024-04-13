PostmasterMain(int argc, char *argv[])
{
	int			opt;
	int			status;
	char	   *userDoption = NULL;
	bool		listen_addr_saved = false;
	int			i;
	char	   *output_config_variable = NULL;

	InitProcessGlobals();

	PostmasterPid = MyProcPid;

	IsPostmasterEnvironment = true;

	/*
	 * Start our win32 signal implementation
	 */
#ifdef WIN32
	pgwin32_signal_initialize();
#endif

	/*
	 * We should not be creating any files or directories before we check the
	 * data directory (see checkDataDir()), but just in case set the umask to
	 * the most restrictive (owner-only) permissions.
	 *
	 * checkDataDir() will reset the umask based on the data directory
	 * permissions.
	 */
	umask(PG_MODE_MASK_OWNER);

	/*
	 * By default, palloc() requests in the postmaster will be allocated in
	 * the PostmasterContext, which is space that can be recycled by backends.
	 * Allocated data that needs to be available to backends should be
	 * allocated in TopMemoryContext.
	 */
	PostmasterContext = AllocSetContextCreate(TopMemoryContext,
											  "Postmaster",
											  ALLOCSET_DEFAULT_SIZES);
	MemoryContextSwitchTo(PostmasterContext);

	/* Initialize paths to installation files */
	getInstallationPaths(argv[0]);

	/*
	 * Set up signal handlers for the postmaster process.
	 *
	 * In the postmaster, we use pqsignal_pm() rather than pqsignal() (which
	 * is used by all child processes and client processes).  That has a
	 * couple of special behaviors:
	 *
	 * 1. Except on Windows, we tell sigaction() to block all signals for the
	 * duration of the signal handler.  This is faster than our old approach
	 * of blocking/unblocking explicitly in the signal handler, and it should
	 * also prevent excessive stack consumption if signals arrive quickly.
	 *
	 * 2. We do not set the SA_RESTART flag.  This is because signals will be
	 * blocked at all times except when ServerLoop is waiting for something to
	 * happen, and during that window, we want signals to exit the select(2)
	 * wait so that ServerLoop can respond if anything interesting happened.
	 * On some platforms, signals marked SA_RESTART would not cause the
	 * select() wait to end.
	 *
	 * Child processes will generally want SA_RESTART, so pqsignal() sets that
	 * flag.  We expect children to set up their own handlers before
	 * unblocking signals.
	 *
	 * CAUTION: when changing this list, check for side-effects on the signal
	 * handling setup of child processes.  See tcop/postgres.c,
	 * bootstrap/bootstrap.c, postmaster/bgwriter.c, postmaster/walwriter.c,
	 * postmaster/autovacuum.c, postmaster/pgarch.c, postmaster/pgstat.c,
	 * postmaster/syslogger.c, postmaster/bgworker.c and
	 * postmaster/checkpointer.c.
	 */
	pqinitmask();
	PG_SETMASK(&BlockSig);

	pqsignal_pm(SIGHUP, SIGHUP_handler);	/* reread config file and have
											 * children do same */
	pqsignal_pm(SIGINT, pmdie); /* send SIGTERM and shut down */
	pqsignal_pm(SIGQUIT, pmdie);	/* send SIGQUIT and die */
	pqsignal_pm(SIGTERM, pmdie);	/* wait for children and shut down */
	pqsignal_pm(SIGALRM, SIG_IGN);	/* ignored */
	pqsignal_pm(SIGPIPE, SIG_IGN);	/* ignored */
	pqsignal_pm(SIGUSR1, sigusr1_handler);	/* message from child process */
	pqsignal_pm(SIGUSR2, dummy_handler);	/* unused, reserve for children */
	pqsignal_pm(SIGCHLD, reaper);	/* handle child termination */

#ifdef SIGURG

	/*
	 * Ignore SIGURG for now.  Child processes may change this (see
	 * InitializeLatchSupport), but they will not receive any such signals
	 * until they wait on a latch.
	 */
	pqsignal_pm(SIGURG, SIG_IGN);	/* ignored */
#endif

	/*
	 * No other place in Postgres should touch SIGTTIN/SIGTTOU handling.  We
	 * ignore those signals in a postmaster environment, so that there is no
	 * risk of a child process freezing up due to writing to stderr.  But for
	 * a standalone backend, their default handling is reasonable.  Hence, all
	 * child processes should just allow the inherited settings to stand.
	 */
#ifdef SIGTTIN
	pqsignal_pm(SIGTTIN, SIG_IGN);	/* ignored */
#endif
#ifdef SIGTTOU
	pqsignal_pm(SIGTTOU, SIG_IGN);	/* ignored */
#endif

	/* ignore SIGXFSZ, so that ulimit violations work like disk full */
#ifdef SIGXFSZ
	pqsignal_pm(SIGXFSZ, SIG_IGN);	/* ignored */
#endif

	/*
	 * Options setup
	 */
	InitializeGUCOptions();

	opterr = 1;

	/*
	 * Parse command-line options.  CAUTION: keep this in sync with
	 * tcop/postgres.c (the option sets should not conflict) and with the
	 * common help() function in main/main.c.
	 */
	while ((opt = getopt(argc, argv, "B:bc:C:D:d:EeFf:h:ijk:lN:nOPp:r:S:sTt:W:-:")) != -1)
	{
		switch (opt)
		{
			case 'B':
				SetConfigOption("shared_buffers", optarg, PGC_POSTMASTER, PGC_S_ARGV);
				break;

			case 'b':
				/* Undocumented flag used for binary upgrades */
				IsBinaryUpgrade = true;
				break;

			case 'C':
				output_config_variable = strdup(optarg);
				break;

			case 'D':
				userDoption = strdup(optarg);
				break;

			case 'd':
				set_debug_options(atoi(optarg), PGC_POSTMASTER, PGC_S_ARGV);
				break;

			case 'E':
				SetConfigOption("log_statement", "all", PGC_POSTMASTER, PGC_S_ARGV);
				break;

			case 'e':
				SetConfigOption("datestyle", "euro", PGC_POSTMASTER, PGC_S_ARGV);
				break;

			case 'F':
				SetConfigOption("fsync", "false", PGC_POSTMASTER, PGC_S_ARGV);
				break;

			case 'f':
				if (!set_plan_disabling_options(optarg, PGC_POSTMASTER, PGC_S_ARGV))
				{
					write_stderr("%s: invalid argument for option -f: \"%s\"\n",
								 progname, optarg);
					ExitPostmaster(1);
				}
				break;

			case 'h':
				SetConfigOption("listen_addresses", optarg, PGC_POSTMASTER, PGC_S_ARGV);
				break;

			case 'i':
				SetConfigOption("listen_addresses", "*", PGC_POSTMASTER, PGC_S_ARGV);
				break;

			case 'j':
				/* only used by interactive backend */
				break;

			case 'k':
				SetConfigOption("unix_socket_directories", optarg, PGC_POSTMASTER, PGC_S_ARGV);
				break;

			case 'l':
				SetConfigOption("ssl", "true", PGC_POSTMASTER, PGC_S_ARGV);
				break;

			case 'N':
				SetConfigOption("max_connections", optarg, PGC_POSTMASTER, PGC_S_ARGV);
				break;

			case 'n':
				/* Don't reinit shared mem after abnormal exit */
				Reinit = false;
				break;

			case 'O':
				SetConfigOption("allow_system_table_mods", "true", PGC_POSTMASTER, PGC_S_ARGV);
				break;

			case 'P':
				SetConfigOption("ignore_system_indexes", "true", PGC_POSTMASTER, PGC_S_ARGV);
				break;

			case 'p':
				SetConfigOption("port", optarg, PGC_POSTMASTER, PGC_S_ARGV);
				break;

			case 'r':
				/* only used by single-user backend */
				break;

			case 'S':
				SetConfigOption("work_mem", optarg, PGC_POSTMASTER, PGC_S_ARGV);
				break;

			case 's':
				SetConfigOption("log_statement_stats", "true", PGC_POSTMASTER, PGC_S_ARGV);
				break;

			case 'T':

				/*
				 * In the event that some backend dumps core, send SIGSTOP,
				 * rather than SIGQUIT, to all its peers.  This lets the wily
				 * post_hacker collect core dumps from everyone.
				 */
				SendStop = true;
				break;

			case 't':
				{
					const char *tmp = get_stats_option_name(optarg);

					if (tmp)
					{
						SetConfigOption(tmp, "true", PGC_POSTMASTER, PGC_S_ARGV);
					}
					else
					{
						write_stderr("%s: invalid argument for option -t: \"%s\"\n",
									 progname, optarg);
						ExitPostmaster(1);
					}
					break;
				}

			case 'W':
				SetConfigOption("post_auth_delay", optarg, PGC_POSTMASTER, PGC_S_ARGV);
				break;

			case 'c':
			case '-':
				{
					char	   *name,
							   *value;

					ParseLongOption(optarg, &name, &value);
					if (!value)
					{
						if (opt == '-')
							ereport(ERROR,
									(errcode(ERRCODE_SYNTAX_ERROR),
									 errmsg("--%s requires a value",
											optarg)));
						else
							ereport(ERROR,
									(errcode(ERRCODE_SYNTAX_ERROR),
									 errmsg("-c %s requires a value",
											optarg)));
					}

					SetConfigOption(name, value, PGC_POSTMASTER, PGC_S_ARGV);
					free(name);
					if (value)
						free(value);
					break;
				}

			default:
				write_stderr("Try \"%s --help\" for more information.\n",
							 progname);
				ExitPostmaster(1);
		}
	}

	/*
	 * Postmaster accepts no non-option switch arguments.
	 */
	if (optind < argc)
	{
		write_stderr("%s: invalid argument: \"%s\"\n",
					 progname, argv[optind]);
		write_stderr("Try \"%s --help\" for more information.\n",
					 progname);
		ExitPostmaster(1);
	}

	/*
	 * Locate the proper configuration files and data directory, and read
	 * postgresql.conf for the first time.
	 */
	if (!SelectConfigFiles(userDoption, progname))
		ExitPostmaster(2);

	if (output_config_variable != NULL)
	{
		/*
		 * If this is a runtime-computed GUC, it hasn't yet been initialized,
		 * and the present value is not useful.  However, this is a convenient
		 * place to print the value for most GUCs because it is safe to run
		 * postmaster startup to this point even if the server is already
		 * running.  For the handful of runtime-computed GUCs that we cannot
		 * provide meaningful values for yet, we wait until later in
		 * postmaster startup to print the value.  We won't be able to use -C
		 * on running servers for those GUCs, but using this option now would
		 * lead to incorrect results for them.
		 */
		int			flags = GetConfigOptionFlags(output_config_variable, true);

		if ((flags & GUC_RUNTIME_COMPUTED) == 0)
		{
			/*
			 * "-C guc" was specified, so print GUC's value and exit.  No
			 * extra permission check is needed because the user is reading
			 * inside the data dir.
			 */
			const char *config_val = GetConfigOption(output_config_variable,
													 false, false);

			puts(config_val ? config_val : "");
			ExitPostmaster(0);
		}
	}

	/* Verify that DataDir looks reasonable */
	checkDataDir();

	/* Check that pg_control exists */
	checkControlFile();

	/* And switch working directory into it */
	ChangeToDataDir();

	/*
	 * Check for invalid combinations of GUC settings.
	 */
	if (ReservedBackends >= MaxConnections)
	{
		write_stderr("%s: superuser_reserved_connections (%d) must be less than max_connections (%d)\n",
					 progname,
					 ReservedBackends, MaxConnections);
		ExitPostmaster(1);
	}
	if (XLogArchiveMode > ARCHIVE_MODE_OFF && wal_level == WAL_LEVEL_MINIMAL)
		ereport(ERROR,
				(errmsg("WAL archival cannot be enabled when wal_level is \"minimal\"")));
	if (max_wal_senders > 0 && wal_level == WAL_LEVEL_MINIMAL)
		ereport(ERROR,
				(errmsg("WAL streaming (max_wal_senders > 0) requires wal_level \"replica\" or \"logical\"")));

	/*
	 * Other one-time internal sanity checks can go here, if they are fast.
	 * (Put any slow processing further down, after postmaster.pid creation.)
	 */
	if (!CheckDateTokenTables())
	{
		write_stderr("%s: invalid datetoken tables, please fix\n", progname);
		ExitPostmaster(1);
	}

	/*
	 * Now that we are done processing the postmaster arguments, reset
	 * getopt(3) library so that it will work correctly in subprocesses.
	 */
	optind = 1;
#ifdef HAVE_INT_OPTRESET
	optreset = 1;				/* some systems need this too */
#endif

	/* For debugging: display postmaster environment */
	{
		extern char **environ;
		char	  **p;

		ereport(DEBUG3,
				(errmsg_internal("%s: PostmasterMain: initial environment dump:",
								 progname)));
		ereport(DEBUG3,
				(errmsg_internal("-----------------------------------------")));
		for (p = environ; *p; ++p)
			ereport(DEBUG3,
					(errmsg_internal("\t%s", *p)));
		ereport(DEBUG3,
				(errmsg_internal("-----------------------------------------")));
	}

	/*
	 * Create lockfile for data directory.
	 *
	 * We want to do this before we try to grab the input sockets, because the
	 * data directory interlock is more reliable than the socket-file
	 * interlock (thanks to whoever decided to put socket files in /tmp :-().
	 * For the same reason, it's best to grab the TCP socket(s) before the
	 * Unix socket(s).
	 *
	 * Also note that this internally sets up the on_proc_exit function that
	 * is responsible for removing both data directory and socket lockfiles;
	 * so it must happen before opening sockets so that at exit, the socket
	 * lockfiles go away after CloseServerPorts runs.
	 */
	CreateDataDirLockFile(true);

	/*
	 * Read the control file (for error checking and config info).
	 *
	 * Since we verify the control file's CRC, this has a useful side effect
	 * on machines where we need a run-time test for CRC support instructions.
	 * The postmaster will do the test once at startup, and then its child
	 * processes will inherit the correct function pointer and not need to
	 * repeat the test.
	 */
	LocalProcessControlFile(false);

	/*
	 * Register the apply launcher.  Since it registers a background worker,
	 * it needs to be called before InitializeMaxBackends(), and it's probably
	 * a good idea to call it before any modules had chance to take the
	 * background worker slots.
	 */
	ApplyLauncherRegister();

	/*
	 * process any libraries that should be preloaded at postmaster start
	 */
	process_shared_preload_libraries();

	/*
	 * Initialize SSL library, if specified.
	 */
#ifdef USE_SSL
	if (EnableSSL)
	{
		(void) secure_initialize(true);
		LoadedSSL = true;
	}
#endif

	/*
	 * Now that loadable modules have had their chance to register background
	 * workers, calculate MaxBackends.
	 */
	InitializeMaxBackends();

	/*
	 * Now that loadable modules have had their chance to request additional
	 * shared memory, determine the value of any runtime-computed GUCs that
	 * depend on the amount of shared memory required.
	 */
	InitializeShmemGUCs();

	/*
	 * If -C was specified with a runtime-computed GUC, we held off printing
	 * the value earlier, as the GUC was not yet initialized.  We handle -C
	 * for most GUCs before we lock the data directory so that the option may
	 * be used on a running server.  However, a handful of GUCs are runtime-
	 * computed and do not have meaningful values until after locking the data
	 * directory, and we cannot safely calculate their values earlier on a
	 * running server.  At this point, such GUCs should be properly
	 * initialized, and we haven't yet set up shared memory, so this is a good
	 * time to handle the -C option for these special GUCs.
	 */
	if (output_config_variable != NULL)
	{
		const char *config_val = GetConfigOption(output_config_variable,
												 false, false);

		puts(config_val ? config_val : "");
		ExitPostmaster(0);
	}

	/*
	 * Set up shared memory and semaphores.
	 */
	reset_shared();

	/*
	 * Estimate number of openable files.  This must happen after setting up
	 * semaphores, because on some platforms semaphores count as open files.
	 */
	set_max_safe_fds();

	/*
	 * Set reference point for stack-depth checking.
	 */
	set_stack_base();

	/*
	 * Initialize pipe (or process handle on Windows) that allows children to
	 * wake up from sleep on postmaster death.
	 */
	InitPostmasterDeathWatchHandle();

#ifdef WIN32

	/*
	 * Initialize I/O completion port used to deliver list of dead children.
	 */
	win32ChildQueue = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 1);
	if (win32ChildQueue == NULL)
		ereport(FATAL,
				(errmsg("could not create I/O completion port for child queue")));
#endif

#ifdef EXEC_BACKEND
	/* Write out nondefault GUC settings for child processes to use */
	write_nondefault_variables(PGC_POSTMASTER);

	/*
	 * Clean out the temp directory used to transmit parameters to child
	 * processes (see internal_forkexec, below).  We must do this before
	 * launching any child processes, else we have a race condition: we could
	 * remove a parameter file before the child can read it.  It should be
	 * safe to do so now, because we verified earlier that there are no
	 * conflicting Postgres processes in this data directory.
	 */
	RemovePgTempFilesInDir(PG_TEMP_FILES_DIR, true, false);
#endif

	/*
	 * Forcibly remove the files signaling a standby promotion request.
	 * Otherwise, the existence of those files triggers a promotion too early,
	 * whether a user wants that or not.
	 *
	 * This removal of files is usually unnecessary because they can exist
	 * only during a few moments during a standby promotion. However there is
	 * a race condition: if pg_ctl promote is executed and creates the files
	 * during a promotion, the files can stay around even after the server is
	 * brought up to be the primary.  Then, if a new standby starts by using
	 * the backup taken from the new primary, the files can exist at server
	 * startup and must be removed in order to avoid an unexpected promotion.
	 *
	 * Note that promotion signal files need to be removed before the startup
	 * process is invoked. Because, after that, they can be used by
	 * postmaster's SIGUSR1 signal handler.
	 */
	RemovePromoteSignalFiles();

	/* Do the same for logrotate signal file */
	RemoveLogrotateSignalFiles();

	/* Remove any outdated file holding the current log filenames. */
	if (unlink(LOG_METAINFO_DATAFILE) < 0 && errno != ENOENT)
		ereport(LOG,
				(errcode_for_file_access(),
				 errmsg("could not remove file \"%s\": %m",
						LOG_METAINFO_DATAFILE)));

	/*
	 * If enabled, start up syslogger collection subprocess
	 */
	SysLoggerPID = SysLogger_Start();

	/*
	 * Reset whereToSendOutput from DestDebug (its starting state) to
	 * DestNone. This stops ereport from sending log messages to stderr unless
	 * Log_destination permits.  We don't do this until the postmaster is
	 * fully launched, since startup failures may as well be reported to
	 * stderr.
	 *
	 * If we are in fact disabling logging to stderr, first emit a log message
	 * saying so, to provide a breadcrumb trail for users who may not remember
	 * that their logging is configured to go somewhere else.
	 */
	if (!(Log_destination & LOG_DESTINATION_STDERR))
		ereport(LOG,
				(errmsg("ending log output to stderr"),
				 errhint("Future log output will go to log destination \"%s\".",
						 Log_destination_string)));

	whereToSendOutput = DestNone;

	/*
	 * Report server startup in log.  While we could emit this much earlier,
	 * it seems best to do so after starting the log collector, if we intend
	 * to use one.
	 */
	ereport(LOG,
			(errmsg("starting %s", PG_VERSION_STR)));

	/*
	 * Establish input sockets.
	 *
	 * First, mark them all closed, and set up an on_proc_exit function that's
	 * charged with closing the sockets again at postmaster shutdown.
	 */
	for (i = 0; i < MAXLISTEN; i++)
		ListenSocket[i] = PGINVALID_SOCKET;

	on_proc_exit(CloseServerPorts, 0);

	if (ListenAddresses)
	{
		char	   *rawstring;
		List	   *elemlist;
		ListCell   *l;
		int			success = 0;

		/* Need a modifiable copy of ListenAddresses */
		rawstring = pstrdup(ListenAddresses);

		/* Parse string into list of hostnames */
		if (!SplitGUCList(rawstring, ',', &elemlist))
		{
			/* syntax error in list */
			ereport(FATAL,
					(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
					 errmsg("invalid list syntax in parameter \"%s\"",
							"listen_addresses")));
		}

		foreach(l, elemlist)
		{
			char	   *curhost = (char *) lfirst(l);

			if (strcmp(curhost, "*") == 0)
				status = StreamServerPort(AF_UNSPEC, NULL,
										  (unsigned short) PostPortNumber,
										  NULL,
										  ListenSocket, MAXLISTEN);
			else
				status = StreamServerPort(AF_UNSPEC, curhost,
										  (unsigned short) PostPortNumber,
										  NULL,
										  ListenSocket, MAXLISTEN);

			if (status == STATUS_OK)
			{
				success++;
				/* record the first successful host addr in lockfile */
				if (!listen_addr_saved)
				{
					AddToDataDirLockFile(LOCK_FILE_LINE_LISTEN_ADDR, curhost);
					listen_addr_saved = true;
				}
			}
			else
				ereport(WARNING,
						(errmsg("could not create listen socket for \"%s\"",
								curhost)));
		}

		if (!success && elemlist != NIL)
			ereport(FATAL,
					(errmsg("could not create any TCP/IP sockets")));

		list_free(elemlist);
		pfree(rawstring);
	}

#ifdef USE_BONJOUR
	/* Register for Bonjour only if we opened TCP socket(s) */
	if (enable_bonjour && ListenSocket[0] != PGINVALID_SOCKET)
	{
		DNSServiceErrorType err;

		/*
		 * We pass 0 for interface_index, which will result in registering on
		 * all "applicable" interfaces.  It's not entirely clear from the
		 * DNS-SD docs whether this would be appropriate if we have bound to
		 * just a subset of the available network interfaces.
		 */
		err = DNSServiceRegister(&bonjour_sdref,
								 0,
								 0,
								 bonjour_name,
								 "_postgresql._tcp.",
								 NULL,
								 NULL,
								 pg_hton16(PostPortNumber),
								 0,
								 NULL,
								 NULL,
								 NULL);
		if (err != kDNSServiceErr_NoError)
			ereport(LOG,
					(errmsg("DNSServiceRegister() failed: error code %ld",
							(long) err)));

		/*
		 * We don't bother to read the mDNS daemon's reply, and we expect that
		 * it will automatically terminate our registration when the socket is
		 * closed at postmaster termination.  So there's nothing more to be
		 * done here.  However, the bonjour_sdref is kept around so that
		 * forked children can close their copies of the socket.
		 */
	}
#endif

#ifdef HAVE_UNIX_SOCKETS
	if (Unix_socket_directories)
	{
		char	   *rawstring;
		List	   *elemlist;
		ListCell   *l;
		int			success = 0;

		/* Need a modifiable copy of Unix_socket_directories */
		rawstring = pstrdup(Unix_socket_directories);

		/* Parse string into list of directories */
		if (!SplitDirectoriesString(rawstring, ',', &elemlist))
		{
			/* syntax error in list */
			ereport(FATAL,
					(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
					 errmsg("invalid list syntax in parameter \"%s\"",
							"unix_socket_directories")));
		}

		foreach(l, elemlist)
		{
			char	   *socketdir = (char *) lfirst(l);

			status = StreamServerPort(AF_UNIX, NULL,
									  (unsigned short) PostPortNumber,
									  socketdir,
									  ListenSocket, MAXLISTEN);

			if (status == STATUS_OK)
			{
				success++;
				/* record the first successful Unix socket in lockfile */
				if (success == 1)
					AddToDataDirLockFile(LOCK_FILE_LINE_SOCKET_DIR, socketdir);
			}
			else
				ereport(WARNING,
						(errmsg("could not create Unix-domain socket in directory \"%s\"",
								socketdir)));
		}

		if (!success && elemlist != NIL)
			ereport(FATAL,
					(errmsg("could not create any Unix-domain sockets")));

		list_free_deep(elemlist);
		pfree(rawstring);
	}
#endif

	/*
	 * check that we have some socket to listen on
	 */
	if (ListenSocket[0] == PGINVALID_SOCKET)
		ereport(FATAL,
				(errmsg("no socket created for listening")));

	/*
	 * If no valid TCP ports, write an empty line for listen address,
	 * indicating the Unix socket must be used.  Note that this line is not
	 * added to the lock file until there is a socket backing it.
	 */
	if (!listen_addr_saved)
		AddToDataDirLockFile(LOCK_FILE_LINE_LISTEN_ADDR, "");

	/*
	 * Record postmaster options.  We delay this till now to avoid recording
	 * bogus options (eg, unusable port number).
	 */
	if (!CreateOptsFile(argc, argv, my_exec_path))
		ExitPostmaster(1);

	/*
	 * Write the external PID file if requested
	 */
	if (external_pid_file)
	{
		FILE	   *fpidfile = fopen(external_pid_file, "w");

		if (fpidfile)
		{
			fprintf(fpidfile, "%d\n", MyProcPid);
			fclose(fpidfile);

			/* Make PID file world readable */
			if (chmod(external_pid_file, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH) != 0)
				write_stderr("%s: could not change permissions of external PID file \"%s\": %s\n",
							 progname, external_pid_file, strerror(errno));
		}
		else
			write_stderr("%s: could not write external PID file \"%s\": %s\n",
						 progname, external_pid_file, strerror(errno));

		on_proc_exit(unlink_external_pid_file, 0);
	}

	/*
	 * Remove old temporary files.  At this point there can be no other
	 * Postgres processes running in this directory, so this should be safe.
	 */
	RemovePgTempFiles();

	/*
	 * Initialize stats collection subsystem (this does NOT start the
	 * collector process!)
	 */
	pgstat_init();

	/*
	 * Initialize the autovacuum subsystem (again, no process start yet)
	 */
	autovac_init();

	/*
	 * Load configuration files for client authentication.
	 */
	if (!load_hba())
	{
		/*
		 * It makes no sense to continue if we fail to load the HBA file,
		 * since there is no way to connect to the database in this case.
		 */
		ereport(FATAL,
				(errmsg("could not load pg_hba.conf")));
	}
	if (!load_ident())
	{
		/*
		 * We can start up without the IDENT file, although it means that you
		 * cannot log in using any of the authentication methods that need a
		 * user name mapping. load_ident() already logged the details of error
		 * to the log.
		 */
	}

#ifdef HAVE_PTHREAD_IS_THREADED_NP

	/*
	 * On macOS, libintl replaces setlocale() with a version that calls
	 * CFLocaleCopyCurrent() when its second argument is "" and every relevant
	 * environment variable is unset or empty.  CFLocaleCopyCurrent() makes
	 * the process multithreaded.  The postmaster calls sigprocmask() and
	 * calls fork() without an immediate exec(), both of which have undefined
	 * behavior in a multithreaded program.  A multithreaded postmaster is the
	 * normal case on Windows, which offers neither fork() nor sigprocmask().
	 */
	if (pthread_is_threaded_np() != 0)
		ereport(FATAL,
				(errcode(ERRCODE_OBJECT_NOT_IN_PREREQUISITE_STATE),
				 errmsg("postmaster became multithreaded during startup"),
				 errhint("Set the LC_ALL environment variable to a valid locale.")));
#endif

	/*
	 * Remember postmaster startup time
	 */
	PgStartTime = GetCurrentTimestamp();

	/*
	 * Report postmaster status in the postmaster.pid file, to allow pg_ctl to
	 * see what's happening.
	 */
	AddToDataDirLockFile(LOCK_FILE_LINE_PM_STATUS, PM_STATUS_STARTING);

	/* Start bgwriter and checkpointer so they can help with recovery */
	if (CheckpointerPID == 0)
		CheckpointerPID = StartCheckpointer();
	if (BgWriterPID == 0)
		BgWriterPID = StartBackgroundWriter();

	/*
	 * We're ready to rock and roll...
	 */
	StartupPID = StartupDataBase();
	Assert(StartupPID != 0);
	StartupStatus = STARTUP_RUNNING;
	pmState = PM_STARTUP;

	/* Some workers may be scheduled to start now */
	maybe_start_bgworkers();

	status = ServerLoop();

	/*
	 * ServerLoop probably shouldn't ever return, but if it does, close down.
	 */
	ExitPostmaster(status != STATUS_OK);

	abort();					/* not reached */
}