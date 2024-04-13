static int __tipc_nl_add_sk(struct sk_buff *skb, struct netlink_callback *cb,
			    struct tipc_sock *tsk)
{
	struct nlattr *attrs;
	void *hdr;

	hdr = genlmsg_put(skb, NETLINK_CB(cb->skb).portid, cb->nlh->nlmsg_seq,
			  &tipc_genl_family, NLM_F_MULTI, TIPC_NL_SOCK_GET);
	if (!hdr)
		goto msg_cancel;

	attrs = nla_nest_start_noflag(skb, TIPC_NLA_SOCK);
	if (!attrs)
		goto genlmsg_cancel;

	if (__tipc_nl_add_sk_info(skb, tsk))
		goto attr_msg_cancel;

	nla_nest_end(skb, attrs);
	genlmsg_end(skb, hdr);

	return 0;

attr_msg_cancel:
	nla_nest_cancel(skb, attrs);
genlmsg_cancel:
	genlmsg_cancel(skb, hdr);
msg_cancel:
	return -EMSGSIZE;
}