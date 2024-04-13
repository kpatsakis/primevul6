static int smack_inet_conn_request(const struct sock *sk, struct sk_buff *skb,
				   struct request_sock *req)
{
	u16 family = sk->sk_family;
	struct smack_known *skp;
	struct socket_smack *ssp = sk->sk_security;
	struct sockaddr_in addr;
	struct iphdr *hdr;
	struct smack_known *hskp;
	int rc;
	struct smk_audit_info ad;
#ifdef CONFIG_AUDIT
	struct lsm_network_audit net;
#endif

#if IS_ENABLED(CONFIG_IPV6)
	if (family == PF_INET6) {
		/*
		 * Handle mapped IPv4 packets arriving
		 * via IPv6 sockets. Don't set up netlabel
		 * processing on IPv6.
		 */
		if (skb->protocol == htons(ETH_P_IP))
			family = PF_INET;
		else
			return 0;
	}
#endif /* CONFIG_IPV6 */

	/*
	 * If there is a secmark use it rather than the CIPSO label.
	 * If there is no secmark fall back to CIPSO.
	 * The secmark is assumed to reflect policy better.
	 */
	skp = smack_from_skb(skb);
	if (skp == NULL) {
		skp = smack_from_netlbl(sk, family, skb);
		if (skp == NULL)
			skp = &smack_known_huh;
	}

#ifdef CONFIG_AUDIT
	smk_ad_init_net(&ad, __func__, LSM_AUDIT_DATA_NET, &net);
	ad.a.u.net->family = family;
	ad.a.u.net->netif = skb->skb_iif;
	ipv4_skb_to_auditdata(skb, &ad.a, NULL);
#endif
	/*
	 * Receiving a packet requires that the other end be able to write
	 * here. Read access is not required.
	 */
	rc = smk_access(skp, ssp->smk_in, MAY_WRITE, &ad);
	rc = smk_bu_note("IPv4 connect", skp, ssp->smk_in, MAY_WRITE, rc);
	if (rc != 0)
		return rc;

	/*
	 * Save the peer's label in the request_sock so we can later setup
	 * smk_packet in the child socket so that SO_PEERCRED can report it.
	 */
	req->peer_secid = skp->smk_secid;

	/*
	 * We need to decide if we want to label the incoming connection here
	 * if we do we only need to label the request_sock and the stack will
	 * propagate the wire-label to the sock when it is created.
	 */
	hdr = ip_hdr(skb);
	addr.sin_addr.s_addr = hdr->saddr;
	rcu_read_lock();
	hskp = smack_ipv4host_label(&addr);
	rcu_read_unlock();

	if (hskp == NULL)
		rc = netlbl_req_setattr(req, &skp->smk_netlabel);
	else
		netlbl_req_delattr(req);

	return rc;
}