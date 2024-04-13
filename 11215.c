static int tipc_accept(struct socket *sock, struct socket *new_sock, int flags,
		       bool kern)
{
	struct sock *new_sk, *sk = sock->sk;
	struct tipc_sock *new_tsock;
	struct msghdr m = {NULL,};
	struct tipc_msg *msg;
	struct sk_buff *buf;
	long timeo;
	int res;

	lock_sock(sk);

	if (sk->sk_state != TIPC_LISTEN) {
		res = -EINVAL;
		goto exit;
	}
	timeo = sock_rcvtimeo(sk, flags & O_NONBLOCK);
	res = tipc_wait_for_accept(sock, timeo);
	if (res)
		goto exit;

	buf = skb_peek(&sk->sk_receive_queue);

	res = tipc_sk_create(sock_net(sock->sk), new_sock, 0, kern);
	if (res)
		goto exit;
	security_sk_clone(sock->sk, new_sock->sk);

	new_sk = new_sock->sk;
	new_tsock = tipc_sk(new_sk);
	msg = buf_msg(buf);

	/* we lock on new_sk; but lockdep sees the lock on sk */
	lock_sock_nested(new_sk, SINGLE_DEPTH_NESTING);

	/*
	 * Reject any stray messages received by new socket
	 * before the socket lock was taken (very, very unlikely)
	 */
	tsk_rej_rx_queue(new_sk, TIPC_ERR_NO_PORT);

	/* Connect new socket to it's peer */
	tipc_sk_finish_conn(new_tsock, msg_origport(msg), msg_orignode(msg));

	tsk_set_importance(new_sk, msg_importance(msg));
	if (msg_named(msg)) {
		new_tsock->conn_addrtype = TIPC_SERVICE_ADDR;
		msg_set_nametype(&new_tsock->phdr, msg_nametype(msg));
		msg_set_nameinst(&new_tsock->phdr, msg_nameinst(msg));
	}

	/*
	 * Respond to 'SYN-' by discarding it & returning 'ACK'.
	 * Respond to 'SYN+' by queuing it on new socket & returning 'ACK'.
	 */
	if (!msg_data_sz(msg)) {
		tsk_advance_rx_queue(sk);
	} else {
		__skb_dequeue(&sk->sk_receive_queue);
		__skb_queue_head(&new_sk->sk_receive_queue, buf);
		skb_set_owner_r(buf, new_sk);
	}
	__tipc_sendstream(new_sock, &m, 0);
	release_sock(new_sk);
exit:
	release_sock(sk);
	return res;
}