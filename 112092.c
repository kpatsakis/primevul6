Lock_AF_UNIX(const char *unixSocketDir, const char *unixSocketPath)
{
	/* no lock file for abstract sockets */
	if (unixSocketPath[0] == '@')
		return STATUS_OK;

	/*
	 * Grab an interlock file associated with the socket file.
	 *
	 * Note: there are two reasons for using a socket lock file, rather than
	 * trying to interlock directly on the socket itself.  First, it's a lot
	 * more portable, and second, it lets us remove any pre-existing socket
	 * file without race conditions.
	 */
	CreateSocketLockFile(unixSocketPath, true, unixSocketDir);

	/*
	 * Once we have the interlock, we can safely delete any pre-existing
	 * socket file to avoid failure at bind() time.
	 */
	(void) unlink(unixSocketPath);

	/*
	 * Remember socket file pathnames for later maintenance.
	 */
	sock_paths = lappend(sock_paths, pstrdup(unixSocketPath));

	return STATUS_OK;
}