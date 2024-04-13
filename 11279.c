static void tipc_sk_push_backlog(struct tipc_sock *tsk, bool nagle_ack)
{
	struct sk_buff_head *txq = &tsk->sk.sk_write_queue;
	struct sk_buff *skb = skb_peek_tail(txq);
	struct net *net = sock_net(&tsk->sk);
	u32 dnode = tsk_peer_node(tsk);
	int rc;

	if (nagle_ack) {
		tsk->pkt_cnt += skb_queue_len(txq);
		if (!tsk->pkt_cnt || tsk->msg_acc / tsk->pkt_cnt < 2) {
			tsk->oneway = 0;
			if (tsk->nagle_start < NAGLE_START_MAX)
				tsk->nagle_start *= 2;
			tsk->expect_ack = false;
			pr_debug("tsk %10u: bad nagle %u -> %u, next start %u!\n",
				 tsk->portid, tsk->msg_acc, tsk->pkt_cnt,
				 tsk->nagle_start);
		} else {
			tsk->nagle_start = NAGLE_START_INIT;
			if (skb) {
				msg_set_ack_required(buf_msg(skb));
				tsk->expect_ack = true;
			} else {
				tsk->expect_ack = false;
			}
		}
		tsk->msg_acc = 0;
		tsk->pkt_cnt = 0;
	}

	if (!skb || tsk->cong_link_cnt)
		return;

	/* Do not send SYN again after congestion */
	if (msg_is_syn(buf_msg(skb)))
		return;

	if (tsk->msg_acc)
		tsk->pkt_cnt += skb_queue_len(txq);
	tsk->snt_unacked += tsk->snd_backlog;
	tsk->snd_backlog = 0;
	rc = tipc_node_xmit(net, txq, dnode, tsk->portid);
	if (rc == -ELINKCONG)
		tsk->cong_link_cnt = 1;
}