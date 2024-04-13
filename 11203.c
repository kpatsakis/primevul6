static int __tipc_sendstream(struct socket *sock, struct msghdr *m, size_t dlen)
{
	struct sock *sk = sock->sk;
	DECLARE_SOCKADDR(struct sockaddr_tipc *, dest, m->msg_name);
	long timeout = sock_sndtimeo(sk, m->msg_flags & MSG_DONTWAIT);
	struct sk_buff_head *txq = &sk->sk_write_queue;
	struct tipc_sock *tsk = tipc_sk(sk);
	struct tipc_msg *hdr = &tsk->phdr;
	struct net *net = sock_net(sk);
	struct sk_buff *skb;
	u32 dnode = tsk_peer_node(tsk);
	int maxnagle = tsk->maxnagle;
	int maxpkt = tsk->max_pkt;
	int send, sent = 0;
	int blocks, rc = 0;

	if (unlikely(dlen > INT_MAX))
		return -EMSGSIZE;

	/* Handle implicit connection setup */
	if (unlikely(dest && sk->sk_state == TIPC_OPEN)) {
		rc = __tipc_sendmsg(sock, m, dlen);
		if (dlen && dlen == rc) {
			tsk->peer_caps = tipc_node_get_capabilities(net, dnode);
			tsk->snt_unacked = tsk_inc(tsk, dlen + msg_hdr_sz(hdr));
		}
		return rc;
	}

	do {
		rc = tipc_wait_for_cond(sock, &timeout,
					(!tsk->cong_link_cnt &&
					 !tsk_conn_cong(tsk) &&
					 tipc_sk_connected(sk)));
		if (unlikely(rc))
			break;
		send = min_t(size_t, dlen - sent, TIPC_MAX_USER_MSG_SIZE);
		blocks = tsk->snd_backlog;
		if (tsk->oneway++ >= tsk->nagle_start && maxnagle &&
		    send <= maxnagle) {
			rc = tipc_msg_append(hdr, m, send, maxnagle, txq);
			if (unlikely(rc < 0))
				break;
			blocks += rc;
			tsk->msg_acc++;
			if (blocks <= 64 && tsk->expect_ack) {
				tsk->snd_backlog = blocks;
				sent += send;
				break;
			} else if (blocks > 64) {
				tsk->pkt_cnt += skb_queue_len(txq);
			} else {
				skb = skb_peek_tail(txq);
				if (skb) {
					msg_set_ack_required(buf_msg(skb));
					tsk->expect_ack = true;
				} else {
					tsk->expect_ack = false;
				}
				tsk->msg_acc = 0;
				tsk->pkt_cnt = 0;
			}
		} else {
			rc = tipc_msg_build(hdr, m, sent, send, maxpkt, txq);
			if (unlikely(rc != send))
				break;
			blocks += tsk_inc(tsk, send + MIN_H_SIZE);
		}
		trace_tipc_sk_sendstream(sk, skb_peek(txq),
					 TIPC_DUMP_SK_SNDQ, " ");
		rc = tipc_node_xmit(net, txq, dnode, tsk->portid);
		if (unlikely(rc == -ELINKCONG)) {
			tsk->cong_link_cnt = 1;
			rc = 0;
		}
		if (likely(!rc)) {
			tsk->snt_unacked += blocks;
			tsk->snd_backlog = 0;
			sent += send;
		}
	} while (sent < dlen && !rc);

	return sent ? sent : rc;
}