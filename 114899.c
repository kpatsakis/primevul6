void notifier_init(notifier_t pipefd)
{
#ifdef WIN32
	SOCKET listener, connecter, acceptor;
	listener = socket(AF_INET, SOCK_STREAM, 0);
	if (listener == INVALID_SOCKET)
		quit(1, "Failed to create listener socket in create_notifier: %s", WindowsErrorStr(WSAGetLastError()));
	connecter = socket(AF_INET, SOCK_STREAM, 0);
	if (connecter == INVALID_SOCKET)
		quit(1, "Failed to create connect socket in create_notifier: %s", WindowsErrorStr(WSAGetLastError()));
	struct sockaddr_in inaddr = {
		.sin_family = AF_INET,
		.sin_addr = {
			.s_addr = htonl(INADDR_LOOPBACK),
		},
		.sin_port = 0,
	};
	{
		char reuse = 1;
		setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
	}
	if (bind(listener, (struct sockaddr*)&inaddr, sizeof(inaddr)) == SOCKET_ERROR)
		quit(1, "Failed to bind listener socket in create_notifier: %s", WindowsErrorStr(WSAGetLastError()));
	socklen_t inaddr_sz = sizeof(inaddr);
	if (getsockname(listener, (struct sockaddr*)&inaddr, &inaddr_sz) == SOCKET_ERROR)
		quit(1, "Failed to getsockname in create_notifier: %s", WindowsErrorStr(WSAGetLastError()));
	if (listen(listener, 1) == SOCKET_ERROR)
		quit(1, "Failed to listen in create_notifier: %s", WindowsErrorStr(WSAGetLastError()));
	inaddr.sin_family = AF_INET;
	inaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	if (connect(connecter, (struct sockaddr*)&inaddr, inaddr_sz) == SOCKET_ERROR)
		quit(1, "Failed to connect in create_notifier: %s", WindowsErrorStr(WSAGetLastError()));
	acceptor = accept(listener, NULL, NULL);
	if (acceptor == INVALID_SOCKET)
		quit(1, "Failed to accept in create_notifier: %s", WindowsErrorStr(WSAGetLastError()));
	closesocket(listener);
	pipefd[0] = connecter;
	pipefd[1] = acceptor;
#else
	if (pipe(pipefd))
		quit(1, "Failed to create pipe in create_notifier");
#endif
}