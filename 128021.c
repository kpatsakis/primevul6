
static const void *net_netlink_ns(struct sock *sk)
{
	return sock_net(sk);