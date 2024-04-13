int DNSRequest::SendRequests(const DNSHeader *header, const int length, QueryType qt)
{
	ServerInstance->Logs->Log("RESOLVER", DEBUG,"DNSRequest::SendRequests");

	unsigned char payload[sizeof(DNSHeader)];

	this->rr_class = 1;
	this->type = qt;

	DNS::EmptyHeader(payload,header,length);

	if (ServerInstance->SE->SendTo(dnsobj, payload, length + 12, 0, &(dnsobj->myserver.sa), sa_size(dnsobj->myserver)) != length+12)
		return -1;

	ServerInstance->Logs->Log("RESOLVER",DEBUG,"Sent OK");
	return 0;
}