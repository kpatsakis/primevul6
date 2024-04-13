static bool tipc_sk_filter_connect(struct tipc_sock *tsk, struct sk_buff *skb,
				   struct sk_buff_head *xmitq)
{
	struct sock *sk = &tsk->sk;
	struct net *net = sock_net(sk);
	struct tipc_msg *hdr = buf_msg(skb);
	bool con_msg = msg_connected(hdr);
	u32 pport = tsk_peer_port(tsk);
	u32 pnode = tsk_peer_node(tsk);
	u32 oport = msg_origport(hdr);
	u32 onode = msg_orignode(hdr);
	int err = msg_errcode(hdr);
	unsigned long delay;

	if (unlikely(msg_mcast(hdr)))
		return false;
	tsk->oneway = 0;

	switch (sk->sk_state) {
	case TIPC_CONNECTING:
		/* Setup ACK */
		if (likely(con_msg)) {
			if (err)
				break;
			tipc_sk_finish_conn(tsk, oport, onode);
			msg_set_importance(&tsk->phdr, msg_importance(hdr));
			/* ACK+ message with data is added to receive queue */
			if (msg_data_sz(hdr))
				return true;
			/* Empty ACK-, - wake up sleeping connect() and drop */
			sk->sk_state_change(sk);
			msg_set_dest_droppable(hdr, 1);
			return false;
		}
		/* Ignore connectionless message if not from listening socket */
		if (oport != pport || onode != pnode)
			return false;

		/* Rejected SYN */
		if (err != TIPC_ERR_OVERLOAD)
			break;

		/* Prepare for new setup attempt if we have a SYN clone */
		if (skb_queue_empty(&sk->sk_write_queue))
			break;
		get_random_bytes(&delay, 2);
		delay %= (tsk->conn_timeout / 4);
		delay = msecs_to_jiffies(delay + 100);
		sk_reset_timer(sk, &sk->sk_timer, jiffies + delay);
		return false;
	case TIPC_OPEN:
	case TIPC_DISCONNECTING:
		return false;
	case TIPC_LISTEN:
		/* Accept only SYN message */
		if (!msg_is_syn(hdr) &&
		    tipc_node_get_capabilities(net, onode) & TIPC_SYN_BIT)
			return false;
		if (!con_msg && !err)
			return true;
		return false;
	case TIPC_ESTABLISHED:
		if (!skb_queue_empty(&sk->sk_write_queue))
			tipc_sk_push_backlog(tsk, false);
		/* Accept only connection-based messages sent by peer */
		if (likely(con_msg && !err && pport == oport &&
			   pnode == onode)) {
			if (msg_ack_required(hdr)) {
				struct sk_buff *skb;

				skb = tipc_sk_build_ack(tsk);
				if (skb) {
					msg_set_nagle_ack(buf_msg(skb));
					__skb_queue_tail(xmitq, skb);
				}
			}
			return true;
		}
		if (!tsk_peer_msg(tsk, hdr))
			return false;
		if (!err)
			return true;
		tipc_set_sk_state(sk, TIPC_DISCONNECTING);
		tipc_node_remove_conn(net, pnode, tsk->portid);
		sk->sk_state_change(sk);
		return true;
	default:
		pr_err("Unknown sk_state %u\n", sk->sk_state);
	}
	/* Abort connection setup attempt */
	tipc_set_sk_state(sk, TIPC_DISCONNECTING);
	sk->sk_err = ECONNREFUSED;
	sk->sk_state_change(sk);
	return true;
}