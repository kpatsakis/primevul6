int DNS::GetNameForce(const char *ip, ForceProtocol fp)
{
	char query[128];
	DNSHeader h;
	int id;
	int length;

	if (fp == PROTOCOL_IPV6)
	{
		in6_addr i;
		if (inet_pton(AF_INET6, ip, &i) > 0)
		{
			DNS::MakeIP6Int(query, &i);
		}
		else
		{
			ServerInstance->Logs->Log("RESOLVER",DEBUG,"DNS::GetNameForce IPv6 bad format for '%s'", ip);
			/* Invalid IP address */
			return -1;
		}
	}
	else
	{
		in_addr i;
		if (inet_aton(ip, &i))
		{
			unsigned char* c = (unsigned char*)&i.s_addr;
			sprintf(query,"%d.%d.%d.%d.in-addr.arpa",c[3],c[2],c[1],c[0]);
		}
		else
		{
			ServerInstance->Logs->Log("RESOLVER",DEBUG,"DNS::GetNameForce IPv4 bad format for '%s'", ip);
			/* Invalid IP address */
			return -1;
		}
	}

	length = this->MakePayload(query, DNS_QUERY_PTR, 1, (unsigned char*)&h.payload);
	if (length == -1)
	{
		ServerInstance->Logs->Log("RESOLVER",DEBUG,"DNS::GetNameForce can't query '%s' using '%s' because it's too long", ip, query);
		return -1;
	}

	DNSRequest* req = this->AddQuery(&h, id, ip);

	if (!req)
	{
		ServerInstance->Logs->Log("RESOLVER",DEBUG,"DNS::GetNameForce can't add query (resolver down?)");
		return -1;
	}

	if (req->SendRequests(&h, length, DNS_QUERY_PTR) == -1)
	{
		ServerInstance->Logs->Log("RESOLVER",DEBUG,"DNS::GetNameForce can't send (firewall?)");
		return -1;
	}

	return id;
}