Resolver::Resolver(const std::string &source, QueryType qt, bool &cached, Module* creator) : Creator(creator), input(source), querytype(qt)
{
	ServerInstance->Logs->Log("RESOLVER",DEBUG,"Resolver::Resolver");
	cached = false;

	CQ = ServerInstance->Res->GetCache(source);
	if (CQ)
	{
		time_left = CQ->CalcTTLRemaining();
		if (!time_left)
		{
			ServerInstance->Res->DelCache(source);
		}
		else
		{
			cached = true;
			return;
		}
	}

	switch (querytype)
	{
		case DNS_QUERY_A:
			this->myid = ServerInstance->Res->GetIP(source.c_str());
		break;

		case DNS_QUERY_PTR4:
			querytype = DNS_QUERY_PTR;
			this->myid = ServerInstance->Res->GetNameForce(source.c_str(), PROTOCOL_IPV4);
		break;

		case DNS_QUERY_PTR6:
			querytype = DNS_QUERY_PTR;
			this->myid = ServerInstance->Res->GetNameForce(source.c_str(), PROTOCOL_IPV6);
		break;

		case DNS_QUERY_AAAA:
			this->myid = ServerInstance->Res->GetIP6(source.c_str());
		break;

		case DNS_QUERY_CNAME:
			this->myid = ServerInstance->Res->GetCName(source.c_str());
		break;

		default:
			ServerInstance->Logs->Log("RESOLVER",DEBUG,"DNS request with unknown query type %d", querytype);
			this->myid = -1;
		break;
	}
	if (this->myid == -1)
	{
		throw ModuleException("Resolver: Couldn't get an id to make a request");
	}
	else
	{
		ServerInstance->Logs->Log("RESOLVER",DEBUG,"DNS request id %d", this->myid);
	}
}