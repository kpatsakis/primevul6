	RequestTimeout(unsigned long n, DNSRequest* watching, int id) : Timer(n, ServerInstance->Time()), watch(watching), watchid(id)
	{
	}