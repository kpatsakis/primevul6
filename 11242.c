static int tipc_wait_for_connect(struct socket *sock, long *timeo_p)
{
	DEFINE_WAIT_FUNC(wait, woken_wake_function);
	struct sock *sk = sock->sk;
	int done;

	do {
		int err = sock_error(sk);
		if (err)
			return err;
		if (!*timeo_p)
			return -ETIMEDOUT;
		if (signal_pending(current))
			return sock_intr_errno(*timeo_p);
		if (sk->sk_state == TIPC_DISCONNECTING)
			break;

		add_wait_queue(sk_sleep(sk), &wait);
		done = sk_wait_event(sk, timeo_p, tipc_sk_connected(sk),
				     &wait);
		remove_wait_queue(sk_sleep(sk), &wait);
	} while (!done);
	return 0;
}