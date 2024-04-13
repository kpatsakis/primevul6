void inet6_rt_notify(int event, struct rt6_info *rt, struct nl_info *info)
{
	struct sk_buff *skb;
	struct net *net = info->nl_net;
	u32 seq;
	int err;

	err = -ENOBUFS;
	seq = info->nlh ? info->nlh->nlmsg_seq : 0;

	skb = nlmsg_new(rt6_nlmsg_size(), gfp_any());
	if (!skb)
		goto errout;

	err = rt6_fill_node(net, skb, rt, NULL, NULL, 0,
				event, info->portid, seq, 0, 0, 0);
	if (err < 0) {
		/* -EMSGSIZE implies BUG in rt6_nlmsg_size() */
		WARN_ON(err == -EMSGSIZE);
		kfree_skb(skb);
		goto errout;
	}
	rtnl_notify(skb, net, info->portid, RTNLGRP_IPV6_ROUTE,
		    info->nlh, gfp_any());
	return;
errout:
	if (err < 0)
		rtnl_set_sk_err(net, RTNLGRP_IPV6_ROUTE, err);
}