static int tipc_recvstream(struct socket *sock, struct msghdr *m,
			   size_t buflen, int flags)
{
	struct sock *sk = sock->sk;
	struct tipc_sock *tsk = tipc_sk(sk);
	struct sk_buff *skb;
	struct tipc_msg *hdr;
	struct tipc_skb_cb *skb_cb;
	bool peek = flags & MSG_PEEK;
	int offset, required, copy, copied = 0;
	int hlen, dlen, err, rc;
	long timeout;

	/* Catch invalid receive attempts */
	if (unlikely(!buflen))
		return -EINVAL;

	lock_sock(sk);

	if (unlikely(sk->sk_state == TIPC_OPEN)) {
		rc = -ENOTCONN;
		goto exit;
	}
	required = sock_rcvlowat(sk, flags & MSG_WAITALL, buflen);
	timeout = sock_rcvtimeo(sk, flags & MSG_DONTWAIT);

	do {
		/* Look at first msg in receive queue; wait if necessary */
		rc = tipc_wait_for_rcvmsg(sock, &timeout);
		if (unlikely(rc))
			break;
		skb = skb_peek(&sk->sk_receive_queue);
		skb_cb = TIPC_SKB_CB(skb);
		hdr = buf_msg(skb);
		dlen = msg_data_sz(hdr);
		hlen = msg_hdr_sz(hdr);
		err = msg_errcode(hdr);

		/* Discard any empty non-errored (SYN-) message */
		if (unlikely(!dlen && !err)) {
			tsk_advance_rx_queue(sk);
			continue;
		}

		/* Collect msg meta data, incl. error code and rejected data */
		if (!copied) {
			tipc_sk_set_orig_addr(m, skb);
			rc = tipc_sk_anc_data_recv(m, skb, tsk);
			if (rc)
				break;
			hdr = buf_msg(skb);
		}

		/* Copy data if msg ok, otherwise return error/partial data */
		if (likely(!err)) {
			offset = skb_cb->bytes_read;
			copy = min_t(int, dlen - offset, buflen - copied);
			rc = skb_copy_datagram_msg(skb, hlen + offset, m, copy);
			if (unlikely(rc))
				break;
			copied += copy;
			offset += copy;
			if (unlikely(offset < dlen)) {
				if (!peek)
					skb_cb->bytes_read = offset;
				break;
			}
		} else {
			rc = 0;
			if ((err != TIPC_CONN_SHUTDOWN) && !m->msg_control)
				rc = -ECONNRESET;
			if (copied || rc)
				break;
		}

		if (unlikely(peek))
			break;

		tsk_advance_rx_queue(sk);

		/* Send connection flow control advertisement when applicable */
		tsk->rcv_unacked += tsk_inc(tsk, hlen + dlen);
		if (tsk->rcv_unacked >= tsk->rcv_win / TIPC_ACK_RATE)
			tipc_sk_send_ack(tsk);

		/* Exit if all requested data or FIN/error received */
		if (copied == buflen || err)
			break;

	} while (!skb_queue_empty(&sk->sk_receive_queue) || copied < required);
exit:
	release_sock(sk);
	return copied ? copied : rc;
}