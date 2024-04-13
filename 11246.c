static void tipc_sk_conn_proto_rcv(struct tipc_sock *tsk, struct sk_buff *skb,
				   struct sk_buff_head *inputq,
				   struct sk_buff_head *xmitq)
{
	struct tipc_msg *hdr = buf_msg(skb);
	u32 onode = tsk_own_node(tsk);
	struct sock *sk = &tsk->sk;
	int mtyp = msg_type(hdr);
	bool was_cong;

	/* Ignore if connection cannot be validated: */
	if (!tsk_peer_msg(tsk, hdr)) {
		trace_tipc_sk_drop_msg(sk, skb, TIPC_DUMP_NONE, "@proto_rcv!");
		goto exit;
	}

	if (unlikely(msg_errcode(hdr))) {
		tipc_set_sk_state(sk, TIPC_DISCONNECTING);
		tipc_node_remove_conn(sock_net(sk), tsk_peer_node(tsk),
				      tsk_peer_port(tsk));
		sk->sk_state_change(sk);

		/* State change is ignored if socket already awake,
		 * - convert msg to abort msg and add to inqueue
		 */
		msg_set_user(hdr, TIPC_CRITICAL_IMPORTANCE);
		msg_set_type(hdr, TIPC_CONN_MSG);
		msg_set_size(hdr, BASIC_H_SIZE);
		msg_set_hdr_sz(hdr, BASIC_H_SIZE);
		__skb_queue_tail(inputq, skb);
		return;
	}

	tsk->probe_unacked = false;

	if (mtyp == CONN_PROBE) {
		msg_set_type(hdr, CONN_PROBE_REPLY);
		if (tipc_msg_reverse(onode, &skb, TIPC_OK))
			__skb_queue_tail(xmitq, skb);
		return;
	} else if (mtyp == CONN_ACK) {
		was_cong = tsk_conn_cong(tsk);
		tipc_sk_push_backlog(tsk, msg_nagle_ack(hdr));
		tsk->snt_unacked -= msg_conn_ack(hdr);
		if (tsk->peer_caps & TIPC_BLOCK_FLOWCTL)
			tsk->snd_win = msg_adv_win(hdr);
		if (was_cong && !tsk_conn_cong(tsk))
			sk->sk_write_space(sk);
	} else if (mtyp != CONN_PROBE_REPLY) {
		pr_warn("Received unknown CONN_PROTO msg\n");
	}
exit:
	kfree_skb(skb);
}