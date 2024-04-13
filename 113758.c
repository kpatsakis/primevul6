DNSRequest* DNS::AddQuery(DNSHeader *header, int &id, const char* original)
{
	/* Is the DNS connection down? */
	if (this->GetFd() == -1)
		return NULL;

	/* Create an id */
	do {
		id = ServerInstance->GenRandomInt(DNS::MAX_REQUEST_ID);
	} while (requests[id]);

	DNSRequest* req = new DNSRequest(this, id, original);

	header->id[0] = req->id[0] = id >> 8;
	header->id[1] = req->id[1] = id & 0xFF;
	header->flags1 = FLAGS_MASK_RD;
	header->flags2 = 0;
	header->qdcount = 1;
	header->ancount = 0;
	header->nscount = 0;
	header->arcount = 0;

	/* At this point we already know the id doesnt exist,
	 * so there needs to be no second check for the ::end()
	 */
	requests[id] = req;

	/* According to the C++ spec, new never returns NULL. */
	return req;
}