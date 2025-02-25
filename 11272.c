static unsigned int rcvbuf_limit(struct sock *sk, struct sk_buff *skb)
{
	struct tipc_sock *tsk = tipc_sk(sk);
	struct tipc_msg *hdr = buf_msg(skb);

	if (unlikely(msg_in_group(hdr)))
		return READ_ONCE(sk->sk_rcvbuf);

	if (unlikely(!msg_connected(hdr)))
		return READ_ONCE(sk->sk_rcvbuf) << msg_importance(hdr);

	if (likely(tsk->peer_caps & TIPC_BLOCK_FLOWCTL))
		return READ_ONCE(sk->sk_rcvbuf);

	return FLOWCTL_MSG_LIM;
}