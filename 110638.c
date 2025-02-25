static inline int rtnetlink_fill_iwinfo(struct sk_buff *	skb,
					struct net_device *	dev,
					int			type,
					char *			event,
					int			event_len)
{
	struct ifinfomsg *r;
	struct nlmsghdr  *nlh;
	unsigned char	 *b = skb->tail;

	nlh = NLMSG_PUT(skb, 0, 0, type, sizeof(*r));
	r = NLMSG_DATA(nlh);
	r->ifi_family = AF_UNSPEC;
	r->ifi_type = dev->type;
	r->ifi_index = dev->ifindex;
	r->ifi_flags = dev->flags;
	r->ifi_change = 0;	/* Wireless changes don't affect those flags */

	/* Add the wireless events in the netlink packet */
	RTA_PUT(skb, IFLA_WIRELESS,
		event_len, event);

	nlh->nlmsg_len = skb->tail - b;
	return skb->len;

nlmsg_failure:
rtattr_failure:
	skb_trim(skb, b - skb->data);
	return -1;
}