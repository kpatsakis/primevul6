int DNS::GetIP(const char *name)
{
	DNSHeader h;
	int id;
	int length;

	if ((length = this->MakePayload(name, DNS_QUERY_A, 1, (unsigned char*)&h.payload)) == -1)
		return -1;

	DNSRequest* req = this->AddQuery(&h, id, name);

	if ((!req) || (req->SendRequests(&h, length, DNS_QUERY_A) == -1))
		return -1;

	return id;
}