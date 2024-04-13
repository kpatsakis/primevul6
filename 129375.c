int rt6_dump_route(struct rt6_info *rt, void *p_arg)
{
	struct rt6_rtnl_dump_arg *arg = (struct rt6_rtnl_dump_arg *) p_arg;
	int prefix;

	if (nlmsg_len(arg->cb->nlh) >= sizeof(struct rtmsg)) {
		struct rtmsg *rtm = nlmsg_data(arg->cb->nlh);
		prefix = (rtm->rtm_flags & RTM_F_PREFIX) != 0;
	} else
		prefix = 0;

	return rt6_fill_node(arg->net,
		     arg->skb, rt, NULL, NULL, 0, RTM_NEWROUTE,
		     NETLINK_CB(arg->cb->skb).portid, arg->cb->nlh->nlmsg_seq,
		     prefix, 0, NLM_F_MULTI);
}