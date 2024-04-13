int tipc_nl_sk_dump(struct sk_buff *skb, struct netlink_callback *cb)
{
	return tipc_nl_sk_walk(skb, cb, __tipc_nl_add_sk);
}