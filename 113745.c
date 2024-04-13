void DNS::MakeIP6Int(char* query, const in6_addr *ip)
{
	const char* hex = "0123456789abcdef";
	for (int index = 31; index >= 0; index--) /* for() loop steps twice per byte */
	{
		if (index % 2)
			/* low nibble */
			*query++ = hex[ip->s6_addr[index / 2] & 0x0F];
		else
			/* high nibble */
			*query++ = hex[(ip->s6_addr[index / 2] & 0xF0) >> 4];
		*query++ = '.'; /* Seperator */
	}
	strcpy(query,"ip6.arpa"); /* Suffix the string */
}