int tipc_dump_start(struct netlink_callback *cb)
{
	return __tipc_dump_start(cb, sock_net(cb->skb->sk));
}