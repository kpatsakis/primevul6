static unsigned int ip6_default_advmss(const struct dst_entry *dst)
{
	struct net_device *dev = dst->dev;
	unsigned int mtu = dst_mtu(dst);
	struct net *net = dev_net(dev);

	mtu -= sizeof(struct ipv6hdr) + sizeof(struct tcphdr);

	if (mtu < net->ipv6.sysctl.ip6_rt_min_advmss)
		mtu = net->ipv6.sysctl.ip6_rt_min_advmss;

	/*
	 * Maximal non-jumbo IPv6 payload is IPV6_MAXPLEN and
	 * corresponding MSS is IPV6_MAXPLEN - tcp_header_size.
	 * IPV6_MAXPLEN is also valid and means: "any MSS,
	 * rely only on pmtu discovery"
	 */
	if (mtu > IPV6_MAXPLEN - sizeof(struct tcphdr))
		mtu = IPV6_MAXPLEN;
	return mtu;
}