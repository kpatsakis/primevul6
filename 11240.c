static void tipc_sk_send_ack(struct tipc_sock *tsk)
{
	struct sk_buff *skb;

	skb = tipc_sk_build_ack(tsk);
	if (!skb)
		return;

	tipc_node_xmit_skb(sock_net(&tsk->sk), skb, tsk_peer_node(tsk),
			   msg_link_selector(buf_msg(skb)));
}