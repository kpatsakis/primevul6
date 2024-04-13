void tipc_socket_stop(void)
{
	sock_unregister(tipc_family_ops.family);
	proto_unregister(&tipc_proto);
}