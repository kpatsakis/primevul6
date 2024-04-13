static bool smk_ipv6_localhost(struct sockaddr_in6 *sip)
{
	__be16 *be16p = (__be16 *)&sip->sin6_addr;
	__be32 *be32p = (__be32 *)&sip->sin6_addr;

	if (be32p[0] == 0 && be32p[1] == 0 && be32p[2] == 0 && be16p[6] == 0 &&
	    ntohs(be16p[7]) == 1)
		return true;
	return false;
}