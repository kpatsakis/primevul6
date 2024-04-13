static int smk_skb_to_addr_ipv6(struct sk_buff *skb, struct sockaddr_in6 *sip)
{
	u8 nexthdr;
	int offset;
	int proto = -EINVAL;
	struct ipv6hdr _ipv6h;
	struct ipv6hdr *ip6;
	__be16 frag_off;
	struct tcphdr _tcph, *th;
	struct udphdr _udph, *uh;
	struct dccp_hdr _dccph, *dh;

	sip->sin6_port = 0;

	offset = skb_network_offset(skb);
	ip6 = skb_header_pointer(skb, offset, sizeof(_ipv6h), &_ipv6h);
	if (ip6 == NULL)
		return -EINVAL;
	sip->sin6_addr = ip6->saddr;

	nexthdr = ip6->nexthdr;
	offset += sizeof(_ipv6h);
	offset = ipv6_skip_exthdr(skb, offset, &nexthdr, &frag_off);
	if (offset < 0)
		return -EINVAL;

	proto = nexthdr;
	switch (proto) {
	case IPPROTO_TCP:
		th = skb_header_pointer(skb, offset, sizeof(_tcph), &_tcph);
		if (th != NULL)
			sip->sin6_port = th->source;
		break;
	case IPPROTO_UDP:
	case IPPROTO_UDPLITE:
		uh = skb_header_pointer(skb, offset, sizeof(_udph), &_udph);
		if (uh != NULL)
			sip->sin6_port = uh->source;
		break;
	case IPPROTO_DCCP:
		dh = skb_header_pointer(skb, offset, sizeof(_dccph), &_dccph);
		if (dh != NULL)
			sip->sin6_port = dh->dccph_sport;
		break;
	}
	return proto;
}