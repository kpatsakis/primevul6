static bool tsk_peer_msg(struct tipc_sock *tsk, struct tipc_msg *msg)
{
	struct sock *sk = &tsk->sk;
	u32 self = tipc_own_addr(sock_net(sk));
	u32 peer_port = tsk_peer_port(tsk);
	u32 orig_node, peer_node;

	if (unlikely(!tipc_sk_connected(sk)))
		return false;

	if (unlikely(msg_origport(msg) != peer_port))
		return false;

	orig_node = msg_orignode(msg);
	peer_node = tsk_peer_node(tsk);

	if (likely(orig_node == peer_node))
		return true;

	if (!orig_node && peer_node == self)
		return true;

	if (!peer_node && orig_node == self)
		return true;

	return false;
}