static void __tipc_shutdown(struct socket *sock, int error)
{
	struct sock *sk = sock->sk;
	struct tipc_sock *tsk = tipc_sk(sk);
	struct net *net = sock_net(sk);
	long timeout = msecs_to_jiffies(CONN_TIMEOUT_DEFAULT);
	u32 dnode = tsk_peer_node(tsk);
	struct sk_buff *skb;

	/* Avoid that hi-prio shutdown msgs bypass msgs in link wakeup queue */
	tipc_wait_for_cond(sock, &timeout, (!tsk->cong_link_cnt &&
					    !tsk_conn_cong(tsk)));

	/* Push out delayed messages if in Nagle mode */
	tipc_sk_push_backlog(tsk, false);
	/* Remove pending SYN */
	__skb_queue_purge(&sk->sk_write_queue);

	/* Remove partially received buffer if any */
	skb = skb_peek(&sk->sk_receive_queue);
	if (skb && TIPC_SKB_CB(skb)->bytes_read) {
		__skb_unlink(skb, &sk->sk_receive_queue);
		kfree_skb(skb);
	}

	/* Reject all unreceived messages if connectionless */
	if (tipc_sk_type_connectionless(sk)) {
		tsk_rej_rx_queue(sk, error);
		return;
	}

	switch (sk->sk_state) {
	case TIPC_CONNECTING:
	case TIPC_ESTABLISHED:
		tipc_set_sk_state(sk, TIPC_DISCONNECTING);
		tipc_node_remove_conn(net, dnode, tsk->portid);
		/* Send a FIN+/- to its peer */
		skb = __skb_dequeue(&sk->sk_receive_queue);
		if (skb) {
			__skb_queue_purge(&sk->sk_receive_queue);
			tipc_sk_respond(sk, skb, error);
			break;
		}
		skb = tipc_msg_create(TIPC_CRITICAL_IMPORTANCE,
				      TIPC_CONN_MSG, SHORT_H_SIZE, 0, dnode,
				      tsk_own_node(tsk), tsk_peer_port(tsk),
				      tsk->portid, error);
		if (skb)
			tipc_node_xmit_skb(net, skb, dnode, tsk->portid);
		break;
	case TIPC_LISTEN:
		/* Reject all SYN messages */
		tsk_rej_rx_queue(sk, error);
		break;
	default:
		__skb_queue_purge(&sk->sk_receive_queue);
		break;
	}
}