static int tipc_recvmsg(struct socket *sock, struct msghdr *m,
			size_t buflen,	int flags)
{
	struct sock *sk = sock->sk;
	bool connected = !tipc_sk_type_connectionless(sk);
	struct tipc_sock *tsk = tipc_sk(sk);
	int rc, err, hlen, dlen, copy;
	struct tipc_skb_cb *skb_cb;
	struct sk_buff_head xmitq;
	struct tipc_msg *hdr;
	struct sk_buff *skb;
	bool grp_evt;
	long timeout;

	/* Catch invalid receive requests */
	if (unlikely(!buflen))
		return -EINVAL;

	lock_sock(sk);
	if (unlikely(connected && sk->sk_state == TIPC_OPEN)) {
		rc = -ENOTCONN;
		goto exit;
	}
	timeout = sock_rcvtimeo(sk, flags & MSG_DONTWAIT);

	/* Step rcv queue to first msg with data or error; wait if necessary */
	do {
		rc = tipc_wait_for_rcvmsg(sock, &timeout);
		if (unlikely(rc))
			goto exit;
		skb = skb_peek(&sk->sk_receive_queue);
		skb_cb = TIPC_SKB_CB(skb);
		hdr = buf_msg(skb);
		dlen = msg_data_sz(hdr);
		hlen = msg_hdr_sz(hdr);
		err = msg_errcode(hdr);
		grp_evt = msg_is_grp_evt(hdr);
		if (likely(dlen || err))
			break;
		tsk_advance_rx_queue(sk);
	} while (1);

	/* Collect msg meta data, including error code and rejected data */
	tipc_sk_set_orig_addr(m, skb);
	rc = tipc_sk_anc_data_recv(m, skb, tsk);
	if (unlikely(rc))
		goto exit;
	hdr = buf_msg(skb);

	/* Capture data if non-error msg, otherwise just set return value */
	if (likely(!err)) {
		int offset = skb_cb->bytes_read;

		copy = min_t(int, dlen - offset, buflen);
		rc = skb_copy_datagram_msg(skb, hlen + offset, m, copy);
		if (unlikely(rc))
			goto exit;
		if (unlikely(offset + copy < dlen)) {
			if (flags & MSG_EOR) {
				if (!(flags & MSG_PEEK))
					skb_cb->bytes_read = offset + copy;
			} else {
				m->msg_flags |= MSG_TRUNC;
				skb_cb->bytes_read = 0;
			}
		} else {
			if (flags & MSG_EOR)
				m->msg_flags |= MSG_EOR;
			skb_cb->bytes_read = 0;
		}
	} else {
		copy = 0;
		rc = 0;
		if (err != TIPC_CONN_SHUTDOWN && connected && !m->msg_control) {
			rc = -ECONNRESET;
			goto exit;
		}
	}

	/* Mark message as group event if applicable */
	if (unlikely(grp_evt)) {
		if (msg_grp_evt(hdr) == TIPC_WITHDRAWN)
			m->msg_flags |= MSG_EOR;
		m->msg_flags |= MSG_OOB;
		copy = 0;
	}

	/* Caption of data or error code/rejected data was successful */
	if (unlikely(flags & MSG_PEEK))
		goto exit;

	/* Send group flow control advertisement when applicable */
	if (tsk->group && msg_in_group(hdr) && !grp_evt) {
		__skb_queue_head_init(&xmitq);
		tipc_group_update_rcv_win(tsk->group, tsk_blocks(hlen + dlen),
					  msg_orignode(hdr), msg_origport(hdr),
					  &xmitq);
		tipc_node_distr_xmit(sock_net(sk), &xmitq);
	}

	if (skb_cb->bytes_read)
		goto exit;

	tsk_advance_rx_queue(sk);

	if (likely(!connected))
		goto exit;

	/* Send connection flow control advertisement when applicable */
	tsk->rcv_unacked += tsk_inc(tsk, hlen + dlen);
	if (tsk->rcv_unacked >= tsk->rcv_win / TIPC_ACK_RATE)
		tipc_sk_send_ack(tsk);
exit:
	release_sock(sk);
	return rc ? rc : copy;
}