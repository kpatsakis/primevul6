static bool rt6_need_strict(const struct in6_addr *daddr)
{
	return ipv6_addr_type(daddr) &
		(IPV6_ADDR_MULTICAST | IPV6_ADDR_LINKLOCAL | IPV6_ADDR_LOOPBACK);
}