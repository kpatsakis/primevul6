read_inheritable_socket(SOCKET *dest, InheritableSocket *src)
{
	SOCKET		s;

	if (src->origsocket == PGINVALID_SOCKET || src->origsocket == 0)
	{
		/* Not a real socket! */
		*dest = src->origsocket;
	}
	else
	{
		/* Actual socket, so create from structure */
		s = WSASocket(FROM_PROTOCOL_INFO,
					  FROM_PROTOCOL_INFO,
					  FROM_PROTOCOL_INFO,
					  &src->wsainfo,
					  0,
					  0);
		if (s == INVALID_SOCKET)
		{
			write_stderr("could not create inherited socket: error code %d\n",
						 WSAGetLastError());
			exit(1);
		}
		*dest = s;

		/*
		 * To make sure we don't get two references to the same socket, close
		 * the original one. (This would happen when inheritance actually
		 * works..
		 */
		closesocket(src->origsocket);
	}
}