static inline int default_protocol_stream(int protocol)
{
	return (protocol == IPPROTO_IP || protocol == IPPROTO_TCP ||
		protocol == IPPROTO_MPTCP);
}