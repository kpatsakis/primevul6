static int __tipc_nl_add_sk_con(struct sk_buff *skb, struct tipc_sock *tsk)
{
	u32 peer_node, peer_port;
	u32 conn_type, conn_instance;
	struct nlattr *nest;

	peer_node = tsk_peer_node(tsk);
	peer_port = tsk_peer_port(tsk);
	conn_type = msg_nametype(&tsk->phdr);
	conn_instance = msg_nameinst(&tsk->phdr);
	nest = nla_nest_start_noflag(skb, TIPC_NLA_SOCK_CON);
	if (!nest)
		return -EMSGSIZE;

	if (nla_put_u32(skb, TIPC_NLA_CON_NODE, peer_node))
		goto msg_full;
	if (nla_put_u32(skb, TIPC_NLA_CON_SOCK, peer_port))
		goto msg_full;

	if (tsk->conn_addrtype != 0) {
		if (nla_put_flag(skb, TIPC_NLA_CON_FLAG))
			goto msg_full;
		if (nla_put_u32(skb, TIPC_NLA_CON_TYPE, conn_type))
			goto msg_full;
		if (nla_put_u32(skb, TIPC_NLA_CON_INST, conn_instance))
			goto msg_full;
	}
	nla_nest_end(skb, nest);

	return 0;

msg_full:
	nla_nest_cancel(skb, nest);

	return -EMSGSIZE;
}