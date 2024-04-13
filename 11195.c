static void tipc_sk_timeout(struct timer_list *t)
{
	struct sock *sk = from_timer(sk, t, sk_timer);
	struct tipc_sock *tsk = tipc_sk(sk);
	u32 pnode = tsk_peer_node(tsk);
	struct sk_buff_head list;
	int rc = 0;

	__skb_queue_head_init(&list);
	bh_lock_sock(sk);

	/* Try again later if socket is busy */
	if (sock_owned_by_user(sk)) {
		sk_reset_timer(sk, &sk->sk_timer, jiffies + HZ / 20);
		bh_unlock_sock(sk);
		sock_put(sk);
		return;
	}

	if (sk->sk_state == TIPC_ESTABLISHED)
		tipc_sk_check_probing_state(sk, &list);
	else if (sk->sk_state == TIPC_CONNECTING)
		tipc_sk_retry_connect(sk, &list);

	bh_unlock_sock(sk);

	if (!skb_queue_empty(&list))
		rc = tipc_node_xmit(sock_net(sk), &list, pnode, tsk->portid);

	/* SYN messages may cause link congestion */
	if (rc == -ELINKCONG) {
		tipc_dest_push(&tsk->cong_links, pnode, 0);
		tsk->cong_link_cnt = 1;
	}
	sock_put(sk);
}