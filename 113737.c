DNSRequest::~DNSRequest()
{
	delete[] res;
}