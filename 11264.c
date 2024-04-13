static int tipc_listen(struct socket *sock, int len)
{
	struct sock *sk = sock->sk;
	int res;

	lock_sock(sk);
	res = tipc_set_sk_state(sk, TIPC_LISTEN);
	release_sock(sk);

	return res;
}