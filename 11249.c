static int tipc_sk_create(struct net *net, struct socket *sock,
			  int protocol, int kern)
{
	const struct proto_ops *ops;
	struct sock *sk;
	struct tipc_sock *tsk;
	struct tipc_msg *msg;

	/* Validate arguments */
	if (unlikely(protocol != 0))
		return -EPROTONOSUPPORT;

	switch (sock->type) {
	case SOCK_STREAM:
		ops = &stream_ops;
		break;
	case SOCK_SEQPACKET:
		ops = &packet_ops;
		break;
	case SOCK_DGRAM:
	case SOCK_RDM:
		ops = &msg_ops;
		break;
	default:
		return -EPROTOTYPE;
	}

	/* Allocate socket's protocol area */
	sk = sk_alloc(net, AF_TIPC, GFP_KERNEL, &tipc_proto, kern);
	if (sk == NULL)
		return -ENOMEM;

	tsk = tipc_sk(sk);
	tsk->max_pkt = MAX_PKT_DEFAULT;
	tsk->maxnagle = 0;
	tsk->nagle_start = NAGLE_START_INIT;
	INIT_LIST_HEAD(&tsk->publications);
	INIT_LIST_HEAD(&tsk->cong_links);
	msg = &tsk->phdr;

	/* Finish initializing socket data structures */
	sock->ops = ops;
	sock_init_data(sock, sk);
	tipc_set_sk_state(sk, TIPC_OPEN);
	if (tipc_sk_insert(tsk)) {
		pr_warn("Socket create failed; port number exhausted\n");
		return -EINVAL;
	}

	/* Ensure tsk is visible before we read own_addr. */
	smp_mb();

	tipc_msg_init(tipc_own_addr(net), msg, TIPC_LOW_IMPORTANCE,
		      TIPC_NAMED_MSG, NAMED_H_SIZE, 0);

	msg_set_origport(msg, tsk->portid);
	timer_setup(&sk->sk_timer, tipc_sk_timeout, 0);
	sk->sk_shutdown = 0;
	sk->sk_backlog_rcv = tipc_sk_backlog_rcv;
	sk->sk_rcvbuf = sysctl_tipc_rmem[1];
	sk->sk_data_ready = tipc_data_ready;
	sk->sk_write_space = tipc_write_space;
	sk->sk_destruct = tipc_sock_destruct;
	tsk->conn_timeout = CONN_TIMEOUT_DEFAULT;
	tsk->group_is_open = true;
	atomic_set(&tsk->dupl_rcvcnt, 0);

	/* Start out with safe limits until we receive an advertised window */
	tsk->snd_win = tsk_adv_blocks(RCVBUF_MIN);
	tsk->rcv_win = tsk->snd_win;

	if (tipc_sk_type_connectionless(sk)) {
		tsk_set_unreturnable(tsk, true);
		if (sock->type == SOCK_DGRAM)
			tsk_set_unreliable(tsk, true);
	}
	__skb_queue_head_init(&tsk->mc_method.deferredq);
	trace_tipc_sk_create(sk, NULL, TIPC_DUMP_NONE, " ");
	return 0;
}