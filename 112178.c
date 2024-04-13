ClosePostmasterPorts(bool am_syslogger)
{
	int			i;

#ifndef WIN32

	/*
	 * Close the write end of postmaster death watch pipe. It's important to
	 * do this as early as possible, so that if postmaster dies, others won't
	 * think that it's still running because we're holding the pipe open.
	 */
	if (close(postmaster_alive_fds[POSTMASTER_FD_OWN]) != 0)
		ereport(FATAL,
				(errcode_for_file_access(),
				 errmsg_internal("could not close postmaster death monitoring pipe in child process: %m")));
	postmaster_alive_fds[POSTMASTER_FD_OWN] = -1;
	/* Notify fd.c that we released one pipe FD. */
	ReleaseExternalFD();
#endif

	/*
	 * Close the postmaster's listen sockets.  These aren't tracked by fd.c,
	 * so we don't call ReleaseExternalFD() here.
	 */
	for (i = 0; i < MAXLISTEN; i++)
	{
		if (ListenSocket[i] != PGINVALID_SOCKET)
		{
			StreamClose(ListenSocket[i]);
			ListenSocket[i] = PGINVALID_SOCKET;
		}
	}

	/*
	 * If using syslogger, close the read side of the pipe.  We don't bother
	 * tracking this in fd.c, either.
	 */
	if (!am_syslogger)
	{
#ifndef WIN32
		if (syslogPipe[0] >= 0)
			close(syslogPipe[0]);
		syslogPipe[0] = -1;
#else
		if (syslogPipe[0])
			CloseHandle(syslogPipe[0]);
		syslogPipe[0] = 0;
#endif
	}

#ifdef USE_BONJOUR
	/* If using Bonjour, close the connection to the mDNS daemon */
	if (bonjour_sdref)
		close(DNSServiceRefSockFD(bonjour_sdref));
#endif
}