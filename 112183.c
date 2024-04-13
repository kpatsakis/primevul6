BackendStartup(Port *port)
{
	Backend    *bn;				/* for backend cleanup */
	pid_t		pid;

	/*
	 * Create backend data structure.  Better before the fork() so we can
	 * handle failure cleanly.
	 */
	bn = (Backend *) malloc(sizeof(Backend));
	if (!bn)
	{
		ereport(LOG,
				(errcode(ERRCODE_OUT_OF_MEMORY),
				 errmsg("out of memory")));
		return STATUS_ERROR;
	}

	/*
	 * Compute the cancel key that will be assigned to this backend. The
	 * backend will have its own copy in the forked-off process' value of
	 * MyCancelKey, so that it can transmit the key to the frontend.
	 */
	if (!RandomCancelKey(&MyCancelKey))
	{
		free(bn);
		ereport(LOG,
				(errcode(ERRCODE_INTERNAL_ERROR),
				 errmsg("could not generate random cancel key")));
		return STATUS_ERROR;
	}

	bn->cancel_key = MyCancelKey;

	/* Pass down canAcceptConnections state */
	port->canAcceptConnections = canAcceptConnections(BACKEND_TYPE_NORMAL);
	bn->dead_end = (port->canAcceptConnections != CAC_OK &&
					port->canAcceptConnections != CAC_SUPERUSER);

	/*
	 * Unless it's a dead_end child, assign it a child slot number
	 */
	if (!bn->dead_end)
		bn->child_slot = MyPMChildSlot = AssignPostmasterChildSlot();
	else
		bn->child_slot = 0;

	/* Hasn't asked to be notified about any bgworkers yet */
	bn->bgworker_notify = false;

#ifdef EXEC_BACKEND
	pid = backend_forkexec(port);
#else							/* !EXEC_BACKEND */
	pid = fork_process();
	if (pid == 0)				/* child */
	{
		free(bn);

		/* Detangle from postmaster */
		InitPostmasterChild();

		/* Close the postmaster's sockets */
		ClosePostmasterPorts(false);

		/* Perform additional initialization and collect startup packet */
		BackendInitialize(port);

		/*
		 * Create a per-backend PGPROC struct in shared memory. We must do
		 * this before we can use LWLocks. In the !EXEC_BACKEND case (here)
		 * this could be delayed a bit further, but EXEC_BACKEND needs to do
		 * stuff with LWLocks before PostgresMain(), so we do it here as well
		 * for symmetry.
		 */
		InitProcess();

		/* And run the backend */
		BackendRun(port);
	}
#endif							/* EXEC_BACKEND */

	if (pid < 0)
	{
		/* in parent, fork failed */
		int			save_errno = errno;

		if (!bn->dead_end)
			(void) ReleasePostmasterChildSlot(bn->child_slot);
		free(bn);
		errno = save_errno;
		ereport(LOG,
				(errmsg("could not fork new process for connection: %m")));
		report_fork_failure_to_client(port, save_errno);
		return STATUS_ERROR;
	}

	/* in parent, successful fork */
	ereport(DEBUG2,
			(errmsg_internal("forked new backend, pid=%d socket=%d",
							 (int) pid, (int) port->sock)));

	/*
	 * Everything's been successful, it's safe to add this backend to our list
	 * of backends.
	 */
	bn->pid = pid;
	bn->bkend_type = BACKEND_TYPE_NORMAL;	/* Can change later to WALSND */
	dlist_push_head(&BackendList, &bn->elem);

#ifdef EXEC_BACKEND
	if (!bn->dead_end)
		ShmemBackendArrayAdd(bn);
#endif

	return STATUS_OK;
}