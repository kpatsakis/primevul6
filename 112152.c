CloseServerPorts(int status, Datum arg)
{
	int			i;

	/*
	 * First, explicitly close all the socket FDs.  We used to just let this
	 * happen implicitly at postmaster exit, but it's better to close them
	 * before we remove the postmaster.pid lockfile; otherwise there's a race
	 * condition if a new postmaster wants to re-use the TCP port number.
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
	 * Next, remove any filesystem entries for Unix sockets.  To avoid race
	 * conditions against incoming postmasters, this must happen after closing
	 * the sockets and before removing lock files.
	 */
	RemoveSocketFiles();

	/*
	 * We don't do anything about socket lock files here; those will be
	 * removed in a later on_proc_exit callback.
	 */
}