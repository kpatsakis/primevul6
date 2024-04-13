void DNS::Rehash()
{
	if (this->GetFd() > -1)
	{
		ServerInstance->SE->DelFd(this);
		ServerInstance->SE->Shutdown(this, 2);
		ServerInstance->SE->Close(this);
		this->SetFd(-1);

		/* Rehash the cache */
		this->PruneCache();
	}
	else
	{
		/* Create initial dns cache */
		this->cache = new dnscache();
	}

	irc::sockets::aptosa(ServerInstance->Config->DNSServer, DNS::QUERY_PORT, myserver);

	/* Initialize mastersocket */
	int s = socket(myserver.sa.sa_family, SOCK_DGRAM, 0);
	this->SetFd(s);

	/* Have we got a socket and is it nonblocking? */
	if (this->GetFd() != -1)
	{
		ServerInstance->SE->SetReuse(s);
		ServerInstance->SE->NonBlocking(s);
		irc::sockets::sockaddrs bindto;
		memset(&bindto, 0, sizeof(bindto));
		bindto.sa.sa_family = myserver.sa.sa_family;
		if (ServerInstance->SE->Bind(this->GetFd(), bindto) < 0)
		{
			/* Failed to bind */
			ServerInstance->Logs->Log("RESOLVER",SPARSE,"Error binding dns socket - hostnames will NOT resolve");
			ServerInstance->SE->Shutdown(this, 2);
			ServerInstance->SE->Close(this);
			this->SetFd(-1);
		}
		else if (!ServerInstance->SE->AddFd(this, FD_WANT_POLL_READ | FD_WANT_NO_WRITE))
		{
			ServerInstance->Logs->Log("RESOLVER",SPARSE,"Internal error starting DNS - hostnames will NOT resolve.");
			ServerInstance->SE->Shutdown(this, 2);
			ServerInstance->SE->Close(this);
			this->SetFd(-1);
		}
	}
	else
	{
		ServerInstance->Logs->Log("RESOLVER",SPARSE,"Error creating DNS socket - hostnames will NOT resolve");
	}
}