static int tipc_shutdown(struct socket *sock, int how)
{
	struct sock *sk = sock->sk;
	int res;

	if (how != SHUT_RDWR)
		return -EINVAL;

	lock_sock(sk);

	trace_tipc_sk_shutdown(sk, NULL, TIPC_DUMP_ALL, " ");
	__tipc_shutdown(sock, TIPC_CONN_SHUTDOWN);
	sk->sk_shutdown = SHUTDOWN_MASK;

	if (sk->sk_state == TIPC_DISCONNECTING) {
		/* Discard any unreceived messages */
		__skb_queue_purge(&sk->sk_receive_queue);

		res = 0;
	} else {
		res = -ENOTCONN;
	}
	/* Wake up anyone sleeping in poll. */
	sk->sk_state_change(sk);

	release_sock(sk);
	return res;
}