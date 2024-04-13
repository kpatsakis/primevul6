static inline int default_protocol_dgram(int protocol)
{
	return (protocol == IPPROTO_IP || protocol == IPPROTO_UDP);
}