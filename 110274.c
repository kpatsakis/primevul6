bool __skb_flow_dissect(const struct sk_buff *skb,
			struct flow_dissector *flow_dissector,
			void *target_container,
			void *data, __be16 proto, int nhoff, int hlen)
{
	struct flow_dissector_key_control *key_control;
	struct flow_dissector_key_basic *key_basic;
	struct flow_dissector_key_addrs *key_addrs;
	struct flow_dissector_key_ports *key_ports;
	struct flow_dissector_key_tags *key_tags;
	struct flow_dissector_key_keyid *key_keyid;
	u8 ip_proto;

	if (!data) {
		data = skb->data;
		proto = skb->protocol;
		nhoff = skb_network_offset(skb);
		hlen = skb_headlen(skb);
	}

	/* It is ensured by skb_flow_dissector_init() that control key will
	 * be always present.
	 */
	key_control = skb_flow_dissector_target(flow_dissector,
						FLOW_DISSECTOR_KEY_CONTROL,
						target_container);

	/* It is ensured by skb_flow_dissector_init() that basic key will
	 * be always present.
	 */
	key_basic = skb_flow_dissector_target(flow_dissector,
					      FLOW_DISSECTOR_KEY_BASIC,
					      target_container);

	if (skb_flow_dissector_uses_key(flow_dissector,
					FLOW_DISSECTOR_KEY_ETH_ADDRS)) {
		struct ethhdr *eth = eth_hdr(skb);
		struct flow_dissector_key_eth_addrs *key_eth_addrs;

		key_eth_addrs = skb_flow_dissector_target(flow_dissector,
							  FLOW_DISSECTOR_KEY_ETH_ADDRS,
							  target_container);
		memcpy(key_eth_addrs, &eth->h_dest, sizeof(*key_eth_addrs));
	}

again:
	switch (proto) {
	case htons(ETH_P_IP): {
		const struct iphdr *iph;
		struct iphdr _iph;
ip:
		iph = __skb_header_pointer(skb, nhoff, sizeof(_iph), data, hlen, &_iph);
		if (!iph || iph->ihl < 5)
			return false;
		nhoff += iph->ihl * 4;

		ip_proto = iph->protocol;
		if (ip_is_fragment(iph))
			ip_proto = 0;

		if (!skb_flow_dissector_uses_key(flow_dissector,
						 FLOW_DISSECTOR_KEY_IPV4_ADDRS))
			break;

		key_addrs = skb_flow_dissector_target(flow_dissector,
			      FLOW_DISSECTOR_KEY_IPV4_ADDRS, target_container);
		memcpy(&key_addrs->v4addrs, &iph->saddr,
		       sizeof(key_addrs->v4addrs));
		key_control->addr_type = FLOW_DISSECTOR_KEY_IPV4_ADDRS;
		break;
	}
	case htons(ETH_P_IPV6): {
		const struct ipv6hdr *iph;
		struct ipv6hdr _iph;
		__be32 flow_label;

ipv6:
		iph = __skb_header_pointer(skb, nhoff, sizeof(_iph), data, hlen, &_iph);
		if (!iph)
			return false;

		ip_proto = iph->nexthdr;
		nhoff += sizeof(struct ipv6hdr);

		if (skb_flow_dissector_uses_key(flow_dissector,
						FLOW_DISSECTOR_KEY_IPV6_ADDRS)) {
			struct flow_dissector_key_ipv6_addrs *key_ipv6_addrs;

			key_ipv6_addrs = skb_flow_dissector_target(flow_dissector,
								   FLOW_DISSECTOR_KEY_IPV6_ADDRS,
								   target_container);

			memcpy(key_ipv6_addrs, &iph->saddr, sizeof(*key_ipv6_addrs));
			key_control->addr_type = FLOW_DISSECTOR_KEY_IPV6_ADDRS;
		}

		flow_label = ip6_flowlabel(iph);
		if (flow_label) {
			if (skb_flow_dissector_uses_key(flow_dissector,
				FLOW_DISSECTOR_KEY_FLOW_LABEL)) {
				key_tags = skb_flow_dissector_target(flow_dissector,
								     FLOW_DISSECTOR_KEY_FLOW_LABEL,
								     target_container);
				key_tags->flow_label = ntohl(flow_label);
			}
		}

		break;
	}
	case htons(ETH_P_8021AD):
	case htons(ETH_P_8021Q): {
		const struct vlan_hdr *vlan;
		struct vlan_hdr _vlan;

		vlan = __skb_header_pointer(skb, nhoff, sizeof(_vlan), data, hlen, &_vlan);
		if (!vlan)
			return false;

		if (skb_flow_dissector_uses_key(flow_dissector,
						FLOW_DISSECTOR_KEY_VLANID)) {
			key_tags = skb_flow_dissector_target(flow_dissector,
							     FLOW_DISSECTOR_KEY_VLANID,
							     target_container);

			key_tags->vlan_id = skb_vlan_tag_get_id(skb);
		}

		proto = vlan->h_vlan_encapsulated_proto;
		nhoff += sizeof(*vlan);
		goto again;
	}
	case htons(ETH_P_PPP_SES): {
		struct {
			struct pppoe_hdr hdr;
			__be16 proto;
		} *hdr, _hdr;
		hdr = __skb_header_pointer(skb, nhoff, sizeof(_hdr), data, hlen, &_hdr);
		if (!hdr)
			return false;
		proto = hdr->proto;
		nhoff += PPPOE_SES_HLEN;
		switch (proto) {
		case htons(PPP_IP):
			goto ip;
		case htons(PPP_IPV6):
			goto ipv6;
		default:
			return false;
		}
	}
	case htons(ETH_P_TIPC): {
		struct {
			__be32 pre[3];
			__be32 srcnode;
		} *hdr, _hdr;
		hdr = __skb_header_pointer(skb, nhoff, sizeof(_hdr), data, hlen, &_hdr);
		if (!hdr)
			return false;
		key_basic->n_proto = proto;
		key_control->thoff = (u16)nhoff;

		if (skb_flow_dissector_uses_key(flow_dissector,
						FLOW_DISSECTOR_KEY_TIPC_ADDRS)) {
			key_addrs = skb_flow_dissector_target(flow_dissector,
							      FLOW_DISSECTOR_KEY_TIPC_ADDRS,
							      target_container);
			key_addrs->tipcaddrs.srcnode = hdr->srcnode;
			key_control->addr_type = FLOW_DISSECTOR_KEY_TIPC_ADDRS;
		}
		return true;
	}
	case htons(ETH_P_FCOE):
		key_control->thoff = (u16)(nhoff + FCOE_HEADER_LEN);
		/* fall through */
	default:
		return false;
	}

	switch (ip_proto) {
	case IPPROTO_GRE: {
		struct gre_hdr {
			__be16 flags;
			__be16 proto;
		} *hdr, _hdr;

		hdr = __skb_header_pointer(skb, nhoff, sizeof(_hdr), data, hlen, &_hdr);
		if (!hdr)
			return false;
		/*
		 * Only look inside GRE if version zero and no
		 * routing
		 */
		if (hdr->flags & (GRE_VERSION | GRE_ROUTING))
			break;

		proto = hdr->proto;
		nhoff += 4;
		if (hdr->flags & GRE_CSUM)
			nhoff += 4;
		if (hdr->flags & GRE_KEY) {
			const __be32 *keyid;
			__be32 _keyid;

			keyid = __skb_header_pointer(skb, nhoff, sizeof(_keyid),
						     data, hlen, &_keyid);

			if (!keyid)
				return false;

			if (skb_flow_dissector_uses_key(flow_dissector,
							FLOW_DISSECTOR_KEY_GRE_KEYID)) {
				key_keyid = skb_flow_dissector_target(flow_dissector,
								      FLOW_DISSECTOR_KEY_GRE_KEYID,
								      target_container);
				key_keyid->keyid = *keyid;
			}
			nhoff += 4;
		}
		if (hdr->flags & GRE_SEQ)
			nhoff += 4;
		if (proto == htons(ETH_P_TEB)) {
			const struct ethhdr *eth;
			struct ethhdr _eth;

			eth = __skb_header_pointer(skb, nhoff,
						   sizeof(_eth),
						   data, hlen, &_eth);
			if (!eth)
				return false;
			proto = eth->h_proto;
			nhoff += sizeof(*eth);
		}
		goto again;
	}
	case IPPROTO_IPIP:
		proto = htons(ETH_P_IP);
		goto ip;
	case IPPROTO_IPV6:
		proto = htons(ETH_P_IPV6);
		goto ipv6;
	default:
		break;
	}

	key_basic->n_proto = proto;
	key_basic->ip_proto = ip_proto;
	key_control->thoff = (u16)nhoff;

	if (skb_flow_dissector_uses_key(flow_dissector,
					FLOW_DISSECTOR_KEY_PORTS)) {
		key_ports = skb_flow_dissector_target(flow_dissector,
						      FLOW_DISSECTOR_KEY_PORTS,
						      target_container);
		key_ports->ports = __skb_flow_get_ports(skb, nhoff, ip_proto,
							data, hlen);
	}

	return true;
}