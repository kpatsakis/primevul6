	virtual void Tick(time_t)
	{
		dns->PruneCache();
	}