	void Tick(time_t)
	{
		if (ServerInstance->Res->requests[watchid] == watch)
		{
			/* Still exists, whack it */
			if (ServerInstance->Res->Classes[watchid])
			{
				ServerInstance->Res->Classes[watchid]->OnError(RESOLVER_TIMEOUT, "Request timed out");
				delete ServerInstance->Res->Classes[watchid];
				ServerInstance->Res->Classes[watchid] = NULL;
			}
			ServerInstance->Res->requests[watchid] = NULL;
			delete watch;
		}
	}