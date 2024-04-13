static void tipc_sk_enqueue(struct sk_buff_head *inputq, struct sock *sk,
			    u32 dport, struct sk_buff_head *xmitq)
{
	unsigned long time_limit = jiffies + usecs_to_jiffies(20000);
	struct sk_buff *skb;
	unsigned int lim;
	atomic_t *dcnt;
	u32 onode;

	while (skb_queue_len(inputq)) {
		if (unlikely(time_after_eq(jiffies, time_limit)))
			return;

		skb = tipc_skb_dequeue(inputq, dport);
		if (unlikely(!skb))
			return;

		/* Add message directly to receive queue if possible */
		if (!sock_owned_by_user(sk)) {
			tipc_sk_filter_rcv(sk, skb, xmitq);
			continue;
		}

		/* Try backlog, compensating for double-counted bytes */
		dcnt = &tipc_sk(sk)->dupl_rcvcnt;
		if (!sk->sk_backlog.len)
			atomic_set(dcnt, 0);
		lim = rcvbuf_limit(sk, skb) + atomic_read(dcnt);
		if (likely(!sk_add_backlog(sk, skb, lim))) {
			trace_tipc_sk_overlimit1(sk, skb, TIPC_DUMP_ALL,
						 "bklg & rcvq >90% allocated!");
			continue;
		}

		trace_tipc_sk_dump(sk, skb, TIPC_DUMP_ALL, "err_overload!");
		/* Overload => reject message back to sender */
		onode = tipc_own_addr(sock_net(sk));
		atomic_inc(&sk->sk_drops);
		if (tipc_msg_reverse(onode, &skb, TIPC_ERR_OVERLOAD)) {
			trace_tipc_sk_rej_msg(sk, skb, TIPC_DUMP_ALL,
					      "@sk_enqueue!");
			__skb_queue_tail(xmitq, skb);
		}
		break;
	}
}