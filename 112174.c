ConnCreate(int serverFd)
{
	Port	   *port;

	if (!(port = (Port *) calloc(1, sizeof(Port))))
	{
		ereport(LOG,
				(errcode(ERRCODE_OUT_OF_MEMORY),
				 errmsg("out of memory")));
		ExitPostmaster(1);
	}

	if (StreamConnection(serverFd, port) != STATUS_OK)
	{
		if (port->sock != PGINVALID_SOCKET)
			StreamClose(port->sock);
		ConnFree(port);
		return NULL;
	}

	return port;
}