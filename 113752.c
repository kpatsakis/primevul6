DNSRequest::DNSRequest(DNS* dns, int rid, const std::string &original) : dnsobj(dns)
{
	/* hardening against overflow here:  make our work buffer twice the theoretical
	 * maximum size so that hostile input doesn't screw us over.
	 */
	res = new unsigned char[sizeof(DNSHeader) * 2];
	*res = 0;
	orig = original;
	RequestTimeout* RT = new RequestTimeout(ServerInstance->Config->dns_timeout ? ServerInstance->Config->dns_timeout : 5, this, rid);
	ServerInstance->Timers->AddTimer(RT); /* The timer manager frees this */
}