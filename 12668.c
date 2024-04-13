static int smk_ipv4_check(struct sock *sk, struct sockaddr_in *sap)
{
	struct smack_known *skp;
	int rc = 0;
	struct smack_known *hkp;
	struct socket_smack *ssp = sk->sk_security;
	struct smk_audit_info ad;

	rcu_read_lock();
	hkp = smack_ipv4host_label(sap);
	if (hkp != NULL) {
#ifdef CONFIG_AUDIT
		struct lsm_network_audit net;

		smk_ad_init_net(&ad, __func__, LSM_AUDIT_DATA_NET, &net);
		ad.a.u.net->family = sap->sin_family;
		ad.a.u.net->dport = sap->sin_port;
		ad.a.u.net->v4info.daddr = sap->sin_addr.s_addr;
#endif
		skp = ssp->smk_out;
		rc = smk_access(skp, hkp, MAY_WRITE, &ad);
		rc = smk_bu_note("IPv4 host check", skp, hkp, MAY_WRITE, rc);
		/*
		 * Clear the socket netlabel if it's set.
		 */
		if (!rc)
			smack_netlbl_delete(sk);
	}
	rcu_read_unlock();

	return rc;
}