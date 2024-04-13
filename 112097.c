BackendInitialize(Port *port)
{
	int			status;
	int			ret;
	char		remote_host[NI_MAXHOST];
	char		remote_port[NI_MAXSERV];
	StringInfoData ps_data;

	/* Save port etc. for ps status */
	MyProcPort = port;

	/* Tell fd.c about the long-lived FD associated with the port */
	ReserveExternalFD();

	/*
	 * PreAuthDelay is a debugging aid for investigating problems in the
	 * authentication cycle: it can be set in postgresql.conf to allow time to
	 * attach to the newly-forked backend with a debugger.  (See also
	 * PostAuthDelay, which we allow clients to pass through PGOPTIONS, but it
	 * is not honored until after authentication.)
	 */
	if (PreAuthDelay > 0)
		pg_usleep(PreAuthDelay * 1000000L);

	/* This flag will remain set until InitPostgres finishes authentication */
	ClientAuthInProgress = true;	/* limit visibility of log messages */

	/* set these to empty in case they are needed before we set them up */
	port->remote_host = "";
	port->remote_port = "";

	/*
	 * Initialize libpq and enable reporting of ereport errors to the client.
	 * Must do this now because authentication uses libpq to send messages.
	 */
	pq_init();					/* initialize libpq to talk to client */
	whereToSendOutput = DestRemote; /* now safe to ereport to client */

	/*
	 * We arrange to do _exit(1) if we receive SIGTERM or timeout while trying
	 * to collect the startup packet; while SIGQUIT results in _exit(2).
	 * Otherwise the postmaster cannot shutdown the database FAST or IMMED
	 * cleanly if a buggy client fails to send the packet promptly.
	 *
	 * Exiting with _exit(1) is only possible because we have not yet touched
	 * shared memory; therefore no outside-the-process state needs to get
	 * cleaned up.
	 */
	pqsignal(SIGTERM, process_startup_packet_die);
	/* SIGQUIT handler was already set up by InitPostmasterChild */
	InitializeTimeouts();		/* establishes SIGALRM handler */
	PG_SETMASK(&StartupBlockSig);

	/*
	 * Get the remote host name and port for logging and status display.
	 */
	remote_host[0] = '\0';
	remote_port[0] = '\0';
	if ((ret = pg_getnameinfo_all(&port->raddr.addr, port->raddr.salen,
								  remote_host, sizeof(remote_host),
								  remote_port, sizeof(remote_port),
								  (log_hostname ? 0 : NI_NUMERICHOST) | NI_NUMERICSERV)) != 0)
		ereport(WARNING,
				(errmsg_internal("pg_getnameinfo_all() failed: %s",
								 gai_strerror(ret))));

	/*
	 * Save remote_host and remote_port in port structure (after this, they
	 * will appear in log_line_prefix data for log messages).
	 */
	port->remote_host = strdup(remote_host);
	port->remote_port = strdup(remote_port);

	/* And now we can issue the Log_connections message, if wanted */
	if (Log_connections)
	{
		if (remote_port[0])
			ereport(LOG,
					(errmsg("connection received: host=%s port=%s",
							remote_host,
							remote_port)));
		else
			ereport(LOG,
					(errmsg("connection received: host=%s",
							remote_host)));
	}

	/*
	 * If we did a reverse lookup to name, we might as well save the results
	 * rather than possibly repeating the lookup during authentication.
	 *
	 * Note that we don't want to specify NI_NAMEREQD above, because then we'd
	 * get nothing useful for a client without an rDNS entry.  Therefore, we
	 * must check whether we got a numeric IPv4 or IPv6 address, and not save
	 * it into remote_hostname if so.  (This test is conservative and might
	 * sometimes classify a hostname as numeric, but an error in that
	 * direction is safe; it only results in a possible extra lookup.)
	 */
	if (log_hostname &&
		ret == 0 &&
		strspn(remote_host, "0123456789.") < strlen(remote_host) &&
		strspn(remote_host, "0123456789ABCDEFabcdef:") < strlen(remote_host))
		port->remote_hostname = strdup(remote_host);

	/*
	 * Ready to begin client interaction.  We will give up and _exit(1) after
	 * a time delay, so that a broken client can't hog a connection
	 * indefinitely.  PreAuthDelay and any DNS interactions above don't count
	 * against the time limit.
	 *
	 * Note: AuthenticationTimeout is applied here while waiting for the
	 * startup packet, and then again in InitPostgres for the duration of any
	 * authentication operations.  So a hostile client could tie up the
	 * process for nearly twice AuthenticationTimeout before we kick him off.
	 *
	 * Note: because PostgresMain will call InitializeTimeouts again, the
	 * registration of STARTUP_PACKET_TIMEOUT will be lost.  This is okay
	 * since we never use it again after this function.
	 */
	RegisterTimeout(STARTUP_PACKET_TIMEOUT, StartupPacketTimeoutHandler);
	enable_timeout_after(STARTUP_PACKET_TIMEOUT, AuthenticationTimeout * 1000);

	/*
	 * Receive the startup packet (which might turn out to be a cancel request
	 * packet).
	 */
	status = ProcessStartupPacket(port, false, false);

	/*
	 * Disable the timeout, and prevent SIGTERM again.
	 */
	disable_timeout(STARTUP_PACKET_TIMEOUT, false);
	PG_SETMASK(&BlockSig);

	/*
	 * As a safety check that nothing in startup has yet performed
	 * shared-memory modifications that would need to be undone if we had
	 * exited through SIGTERM or timeout above, check that no on_shmem_exit
	 * handlers have been registered yet.  (This isn't terribly bulletproof,
	 * since someone might misuse an on_proc_exit handler for shmem cleanup,
	 * but it's a cheap and helpful check.  We cannot disallow on_proc_exit
	 * handlers unfortunately, since pq_init() already registered one.)
	 */
	check_on_shmem_exit_lists_are_empty();

	/*
	 * Stop here if it was bad or a cancel packet.  ProcessStartupPacket
	 * already did any appropriate error reporting.
	 */
	if (status != STATUS_OK)
		proc_exit(0);

	/*
	 * Now that we have the user and database name, we can set the process
	 * title for ps.  It's good to do this as early as possible in startup.
	 */
	initStringInfo(&ps_data);
	if (am_walsender)
		appendStringInfo(&ps_data, "%s ", GetBackendTypeDesc(B_WAL_SENDER));
	appendStringInfo(&ps_data, "%s ", port->user_name);
	if (!am_walsender)
		appendStringInfo(&ps_data, "%s ", port->database_name);
	appendStringInfo(&ps_data, "%s", port->remote_host);
	if (port->remote_port[0] != '\0')
		appendStringInfo(&ps_data, "(%s)", port->remote_port);

	init_ps_display(ps_data.data);
	pfree(ps_data.data);

	set_ps_display("initializing");
}