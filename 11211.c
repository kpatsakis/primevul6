static int __tipc_nl_add_sk_info(struct sk_buff *skb, struct tipc_sock
			  *tsk)
{
	struct net *net = sock_net(skb->sk);
	struct sock *sk = &tsk->sk;

	if (nla_put_u32(skb, TIPC_NLA_SOCK_REF, tsk->portid) ||
	    nla_put_u32(skb, TIPC_NLA_SOCK_ADDR, tipc_own_addr(net)))
		return -EMSGSIZE;

	if (tipc_sk_connected(sk)) {
		if (__tipc_nl_add_sk_con(skb, tsk))
			return -EMSGSIZE;
	} else if (!list_empty(&tsk->publications)) {
		if (nla_put_flag(skb, TIPC_NLA_SOCK_HAS_PUBL))
			return -EMSGSIZE;
	}
	return 0;
}