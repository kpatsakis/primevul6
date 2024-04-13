static void tipc_sk_check_probing_state(struct sock *sk,
					struct sk_buff_head *list)
{
	struct tipc_sock *tsk = tipc_sk(sk);
	u32 pnode = tsk_peer_node(tsk);
	u32 pport = tsk_peer_port(tsk);
	u32 self = tsk_own_node(tsk);
	u32 oport = tsk->portid;
	struct sk_buff *skb;

	if (tsk->probe_unacked) {
		tipc_set_sk_state(sk, TIPC_DISCONNECTING);
		sk->sk_err = ECONNABORTED;
		tipc_node_remove_conn(sock_net(sk), pnode, pport);
		sk->sk_state_change(sk);
		return;
	}
	/* Prepare new probe */
	skb = tipc_msg_create(CONN_MANAGER, CONN_PROBE, INT_H_SIZE, 0,
			      pnode, self, pport, oport, TIPC_OK);
	if (skb)
		__skb_queue_tail(list, skb);
	tsk->probe_unacked = true;
	sk_reset_timer(sk, &sk->sk_timer, jiffies + CONN_PROBING_INTV);
}