static int tipc_sk_sock_err(struct socket *sock, long *timeout)
{
	struct sock *sk = sock->sk;
	int err = sock_error(sk);
	int typ = sock->type;

	if (err)
		return err;
	if (typ == SOCK_STREAM || typ == SOCK_SEQPACKET) {
		if (sk->sk_state == TIPC_DISCONNECTING)
			return -EPIPE;
		else if (!tipc_sk_connected(sk))
			return -ENOTCONN;
	}
	if (!*timeout)
		return -EAGAIN;
	if (signal_pending(current))
		return sock_intr_errno(*timeout);

	return 0;
}