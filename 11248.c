static int tipc_sendstream(struct socket *sock, struct msghdr *m, size_t dsz)
{
	struct sock *sk = sock->sk;
	int ret;

	lock_sock(sk);
	ret = __tipc_sendstream(sock, m, dsz);
	release_sock(sk);

	return ret;
}