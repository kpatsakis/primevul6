	~RequestTimeout()
	{
		if (ServerInstance->Res)
			Tick(0);
	}