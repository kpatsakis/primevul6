static int smack_unix_may_send(struct socket *sock, struct socket *other)
{
	struct socket_smack *ssp = sock->sk->sk_security;
	struct socket_smack *osp = other->sk->sk_security;
	struct smk_audit_info ad;
	int rc;

#ifdef CONFIG_AUDIT
	struct lsm_network_audit net;

	smk_ad_init_net(&ad, __func__, LSM_AUDIT_DATA_NET, &net);
	smk_ad_setfield_u_net_sk(&ad, other->sk);
#endif

	if (smack_privileged(CAP_MAC_OVERRIDE))
		return 0;

	rc = smk_access(ssp->smk_out, osp->smk_in, MAY_WRITE, &ad);
	rc = smk_bu_note("UDS send", ssp->smk_out, osp->smk_in, MAY_WRITE, rc);
	return rc;
}