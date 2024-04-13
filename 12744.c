static int smack_socket_sock_rcv_skb(struct sock *sk, struct sk_buff *skb)
{
	struct socket_smack *ssp = sk->sk_security;
	struct smack_known *skp = NULL;
	int rc = 0;
	struct smk_audit_info ad;
	u16 family = sk->sk_family;
#ifdef CONFIG_AUDIT
	struct lsm_network_audit net;
#endif
#if IS_ENABLED(CONFIG_IPV6)
	struct sockaddr_in6 sadd;
	int proto;

	if (family == PF_INET6 && skb->protocol == htons(ETH_P_IP))
		family = PF_INET;
#endif /* CONFIG_IPV6 */

	switch (family) {
	case PF_INET:
		/*
		 * If there is a secmark use it rather than the CIPSO label.
		 * If there is no secmark fall back to CIPSO.
		 * The secmark is assumed to reflect policy better.
		 */
		skp = smack_from_skb(skb);
		if (skp == NULL) {
			skp = smack_from_netlbl(sk, family, skb);
			if (skp == NULL)
				skp = smack_net_ambient;
		}

#ifdef CONFIG_AUDIT
		smk_ad_init_net(&ad, __func__, LSM_AUDIT_DATA_NET, &net);
		ad.a.u.net->family = family;
		ad.a.u.net->netif = skb->skb_iif;
		ipv4_skb_to_auditdata(skb, &ad.a, NULL);
#endif
		/*
		 * Receiving a packet requires that the other end
		 * be able to write here. Read access is not required.
		 * This is the simplist possible security model
		 * for networking.
		 */
		rc = smk_access(skp, ssp->smk_in, MAY_WRITE, &ad);
		rc = smk_bu_note("IPv4 delivery", skp, ssp->smk_in,
					MAY_WRITE, rc);
		if (rc != 0)
			netlbl_skbuff_err(skb, family, rc, 0);
		break;
#if IS_ENABLED(CONFIG_IPV6)
	case PF_INET6:
		proto = smk_skb_to_addr_ipv6(skb, &sadd);
		if (proto != IPPROTO_UDP && proto != IPPROTO_UDPLITE &&
		    proto != IPPROTO_TCP && proto != IPPROTO_DCCP)
			break;
#ifdef SMACK_IPV6_SECMARK_LABELING
		skp = smack_from_skb(skb);
		if (skp == NULL) {
			if (smk_ipv6_localhost(&sadd))
				break;
			skp = smack_ipv6host_label(&sadd);
			if (skp == NULL)
				skp = smack_net_ambient;
		}
#ifdef CONFIG_AUDIT
		smk_ad_init_net(&ad, __func__, LSM_AUDIT_DATA_NET, &net);
		ad.a.u.net->family = family;
		ad.a.u.net->netif = skb->skb_iif;
		ipv6_skb_to_auditdata(skb, &ad.a, NULL);
#endif /* CONFIG_AUDIT */
		rc = smk_access(skp, ssp->smk_in, MAY_WRITE, &ad);
		rc = smk_bu_note("IPv6 delivery", skp, ssp->smk_in,
					MAY_WRITE, rc);
#endif /* SMACK_IPV6_SECMARK_LABELING */
#ifdef SMACK_IPV6_PORT_LABELING
		rc = smk_ipv6_port_check(sk, &sadd, SMK_RECEIVING);
#endif /* SMACK_IPV6_PORT_LABELING */
		if (rc != 0)
			icmpv6_send(skb, ICMPV6_DEST_UNREACH,
					ICMPV6_ADM_PROHIBITED, 0);
		break;
#endif /* CONFIG_IPV6 */
	}

	return rc;
}