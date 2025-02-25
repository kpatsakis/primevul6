static int inet6_fill_ifinfo(struct sk_buff *skb, struct inet6_dev *idev, 
			     u32 pid, u32 seq, int event, unsigned int flags)
{
	struct net_device	*dev = idev->dev;
	__s32			*array = NULL;
	struct ifinfomsg	*r;
	struct nlmsghdr 	*nlh;
	unsigned char		*b = skb->tail;
	struct rtattr		*subattr;
	__u32			mtu = dev->mtu;
	struct ifla_cacheinfo	ci;

	nlh = NLMSG_NEW(skb, pid, seq, event, sizeof(*r), flags);
	r = NLMSG_DATA(nlh);
	r->ifi_family = AF_INET6;
	r->ifi_type = dev->type;
	r->ifi_index = dev->ifindex;
	r->ifi_flags = dev_get_flags(dev);
	r->ifi_change = 0;

	RTA_PUT(skb, IFLA_IFNAME, strlen(dev->name)+1, dev->name);

	if (dev->addr_len)
		RTA_PUT(skb, IFLA_ADDRESS, dev->addr_len, dev->dev_addr);

	RTA_PUT(skb, IFLA_MTU, sizeof(mtu), &mtu);
	if (dev->ifindex != dev->iflink)
		RTA_PUT(skb, IFLA_LINK, sizeof(int), &dev->iflink);
			
	subattr = (struct rtattr*)skb->tail;

	RTA_PUT(skb, IFLA_PROTINFO, 0, NULL);

	/* return the device flags */
	RTA_PUT(skb, IFLA_INET6_FLAGS, sizeof(__u32), &idev->if_flags);

	/* return interface cacheinfo */
	ci.max_reasm_len = IPV6_MAXPLEN;
	ci.tstamp = (__u32)(TIME_DELTA(idev->tstamp, INITIAL_JIFFIES) / HZ * 100
		    + TIME_DELTA(idev->tstamp, INITIAL_JIFFIES) % HZ * 100 / HZ);
	ci.reachable_time = idev->nd_parms->reachable_time;
	ci.retrans_time = idev->nd_parms->retrans_time;
	RTA_PUT(skb, IFLA_INET6_CACHEINFO, sizeof(ci), &ci);
	
	/* return the device sysctl params */
	if ((array = kmalloc(DEVCONF_MAX * sizeof(*array), GFP_ATOMIC)) == NULL)
		goto rtattr_failure;
	ipv6_store_devconf(&idev->cnf, array, DEVCONF_MAX * sizeof(*array));
	RTA_PUT(skb, IFLA_INET6_CONF, DEVCONF_MAX * sizeof(*array), array);

	/* XXX - Statistics/MC not implemented */
	subattr->rta_len = skb->tail - (u8*)subattr;

	nlh->nlmsg_len = skb->tail - b;
	kfree(array);
	return skb->len;

nlmsg_failure:
rtattr_failure:
	if (array)
		kfree(array);
	skb_trim(skb, b - skb->data);
	return -1;
}