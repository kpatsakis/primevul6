report_fork_failure_to_client(Port *port, int errnum)
{
	char		buffer[1000];
	int			rc;

	/* Format the error message packet (always V2 protocol) */
	snprintf(buffer, sizeof(buffer), "E%s%s\n",
			 _("could not fork new process for connection: "),
			 strerror(errnum));

	/* Set port to non-blocking.  Don't do send() if this fails */
	if (!pg_set_noblock(port->sock))
		return;

	/* We'll retry after EINTR, but ignore all other failures */
	do
	{
		rc = send(port->sock, buffer, strlen(buffer) + 1, 0);
	} while (rc < 0 && errno == EINTR);
}