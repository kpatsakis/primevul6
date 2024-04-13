static void tipc_sk_filter_rcv(struct sock *sk, struct sk_buff *skb,
			       struct sk_buff_head *xmitq)
{
	bool sk_conn = !tipc_sk_type_connectionless(sk);
	struct tipc_sock *tsk = tipc_sk(sk);
	struct tipc_group *grp = tsk->group;
	struct tipc_msg *hdr = buf_msg(skb);
	struct net *net = sock_net(sk);
	struct sk_buff_head inputq;
	int mtyp = msg_type(hdr);
	int limit, err = TIPC_OK;

	trace_tipc_sk_filter_rcv(sk, skb, TIPC_DUMP_ALL, " ");
	TIPC_SKB_CB(skb)->bytes_read = 0;
	__skb_queue_head_init(&inputq);
	__skb_queue_tail(&inputq, skb);

	if (unlikely(!msg_isdata(hdr)))
		tipc_sk_proto_rcv(sk, &inputq, xmitq);

	if (unlikely(grp))
		tipc_group_filter_msg(grp, &inputq, xmitq);

	if (unlikely(!grp) && mtyp == TIPC_MCAST_MSG)
		tipc_mcast_filter_msg(net, &tsk->mc_method.deferredq, &inputq);

	/* Validate and add to receive buffer if there is space */
	while ((skb = __skb_dequeue(&inputq))) {
		hdr = buf_msg(skb);
		limit = rcvbuf_limit(sk, skb);
		if ((sk_conn && !tipc_sk_filter_connect(tsk, skb, xmitq)) ||
		    (!sk_conn && msg_connected(hdr)) ||
		    (!grp && msg_in_group(hdr)))
			err = TIPC_ERR_NO_PORT;
		else if (sk_rmem_alloc_get(sk) + skb->truesize >= limit) {
			trace_tipc_sk_dump(sk, skb, TIPC_DUMP_ALL,
					   "err_overload2!");
			atomic_inc(&sk->sk_drops);
			err = TIPC_ERR_OVERLOAD;
		}

		if (unlikely(err)) {
			if (tipc_msg_reverse(tipc_own_addr(net), &skb, err)) {
				trace_tipc_sk_rej_msg(sk, skb, TIPC_DUMP_NONE,
						      "@filter_rcv!");
				__skb_queue_tail(xmitq, skb);
			}
			err = TIPC_OK;
			continue;
		}
		__skb_queue_tail(&sk->sk_receive_queue, skb);
		skb_set_owner_r(skb, sk);
		trace_tipc_sk_overlimit2(sk, skb, TIPC_DUMP_ALL,
					 "rcvq >90% allocated!");
		sk->sk_data_ready(sk);
	}
}