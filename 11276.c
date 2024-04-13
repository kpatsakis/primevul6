static int tipc_release(struct socket *sock)
{
	struct sock *sk = sock->sk;
	struct tipc_sock *tsk;

	/*
	 * Exit if socket isn't fully initialized (occurs when a failed accept()
	 * releases a pre-allocated child socket that was never used)
	 */
	if (sk == NULL)
		return 0;

	tsk = tipc_sk(sk);
	lock_sock(sk);

	trace_tipc_sk_release(sk, NULL, TIPC_DUMP_ALL, " ");
	__tipc_shutdown(sock, TIPC_ERR_NO_PORT);
	sk->sk_shutdown = SHUTDOWN_MASK;
	tipc_sk_leave(tsk);
	tipc_sk_withdraw(tsk, NULL);
	__skb_queue_purge(&tsk->mc_method.deferredq);
	sk_stop_timer(sk, &sk->sk_timer);
	tipc_sk_remove(tsk);

	sock_orphan(sk);
	/* Reject any messages that accumulated in backlog queue */
	release_sock(sk);
	tipc_dest_list_purge(&tsk->cong_links);
	tsk->cong_link_cnt = 0;
	call_rcu(&tsk->rcu, tipc_sk_callback);
	sock->sk = NULL;

	return 0;
}