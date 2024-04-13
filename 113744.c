void DNS::HandleEvent(EventType, int)
{
	/* Fetch the id and result of the next available packet */
	DNSResult res(0,"",0,"");
	res.id = 0;
	ServerInstance->Logs->Log("RESOLVER",DEBUG,"Handle DNS event");

	res = this->GetResult();

	ServerInstance->Logs->Log("RESOLVER",DEBUG,"Result id %d", res.id);

	/* Is there a usable request id? */
	if (res.id != -1)
	{
		/* Its an error reply */
		if (res.id & ERROR_MASK)
		{
			/* Mask off the error bit */
			res.id -= ERROR_MASK;
			/* Marshall the error to the correct class */
			if (Classes[res.id])
			{
				if (ServerInstance && ServerInstance->stats)
					ServerInstance->stats->statsDnsBad++;
				Classes[res.id]->OnError(RESOLVER_NXDOMAIN, res.result);
				delete Classes[res.id];
				Classes[res.id] = NULL;
			}
			return;
		}
		else
		{
			/* It is a non-error result, marshall the result to the correct class */
			if (Classes[res.id])
			{
				if (ServerInstance && ServerInstance->stats)
					ServerInstance->stats->statsDnsGood++;

				if (!this->GetCache(res.original.c_str()))
					this->cache->insert(std::make_pair(res.original.c_str(), CachedQuery(res.result, res.ttl)));

				Classes[res.id]->OnLookupComplete(res.result, res.ttl, false);
				delete Classes[res.id];
				Classes[res.id] = NULL;
			}
		}

		if (ServerInstance && ServerInstance->stats)
			ServerInstance->stats->statsDns++;
	}
}