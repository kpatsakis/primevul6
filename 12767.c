static int smk_ipv6_check(struct smack_known *subject,
				struct smack_known *object,
				struct sockaddr_in6 *address, int act)
{
#ifdef CONFIG_AUDIT
	struct lsm_network_audit net;
#endif
	struct smk_audit_info ad;
	int rc;

#ifdef CONFIG_AUDIT
	smk_ad_init_net(&ad, __func__, LSM_AUDIT_DATA_NET, &net);
	ad.a.u.net->family = PF_INET6;
	ad.a.u.net->dport = ntohs(address->sin6_port);
	if (act == SMK_RECEIVING)
		ad.a.u.net->v6info.saddr = address->sin6_addr;
	else
		ad.a.u.net->v6info.daddr = address->sin6_addr;
#endif
	rc = smk_access(subject, object, MAY_WRITE, &ad);
	rc = smk_bu_note("IPv6 check", subject, object, MAY_WRITE, rc);
	return rc;
}