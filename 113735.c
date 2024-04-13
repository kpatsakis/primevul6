int DNS::GetCName(const char *alias)
{
	DNSHeader h;
	int id;
	int length;

	if ((length = this->MakePayload(alias, DNS_QUERY_CNAME, 1, (unsigned char*)&h.payload)) == -1)
		return -1;

	DNSRequest* req = this->AddQuery(&h, id, alias);

	if ((!req) || (req->SendRequests(&h, length, DNS_QUERY_CNAME) == -1))
		return -1;

	return id;
}