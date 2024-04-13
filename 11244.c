static __poll_t tipc_poll(struct file *file, struct socket *sock,
			      poll_table *wait)
{
	struct sock *sk = sock->sk;
	struct tipc_sock *tsk = tipc_sk(sk);
	__poll_t revents = 0;

	sock_poll_wait(file, sock, wait);
	trace_tipc_sk_poll(sk, NULL, TIPC_DUMP_ALL, " ");

	if (sk->sk_shutdown & RCV_SHUTDOWN)
		revents |= EPOLLRDHUP | EPOLLIN | EPOLLRDNORM;
	if (sk->sk_shutdown == SHUTDOWN_MASK)
		revents |= EPOLLHUP;

	switch (sk->sk_state) {
	case TIPC_ESTABLISHED:
		if (!tsk->cong_link_cnt && !tsk_conn_cong(tsk))
			revents |= EPOLLOUT;
		fallthrough;
	case TIPC_LISTEN:
	case TIPC_CONNECTING:
		if (!skb_queue_empty_lockless(&sk->sk_receive_queue))
			revents |= EPOLLIN | EPOLLRDNORM;
		break;
	case TIPC_OPEN:
		if (tsk->group_is_open && !tsk->cong_link_cnt)
			revents |= EPOLLOUT;
		if (!tipc_sk_type_connectionless(sk))
			break;
		if (skb_queue_empty_lockless(&sk->sk_receive_queue))
			break;
		revents |= EPOLLIN | EPOLLRDNORM;
		break;
	case TIPC_DISCONNECTING:
		revents = EPOLLIN | EPOLLRDNORM | EPOLLHUP;
		break;
	}
	return revents;
}