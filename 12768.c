static int smack_socket_connect(struct socket *sock, struct sockaddr *sap,
				int addrlen)
{
	int rc = 0;

	if (sock->sk == NULL)
		return 0;
	if (sock->sk->sk_family != PF_INET &&
	    (!IS_ENABLED(CONFIG_IPV6) || sock->sk->sk_family != PF_INET6))
		return 0;
	if (addrlen < offsetofend(struct sockaddr, sa_family))
		return 0;
	if (IS_ENABLED(CONFIG_IPV6) && sap->sa_family == AF_INET6) {
		struct sockaddr_in6 *sip = (struct sockaddr_in6 *)sap;
		struct smack_known *rsp = NULL;

		if (addrlen < SIN6_LEN_RFC2133)
			return 0;
		if (__is_defined(SMACK_IPV6_SECMARK_LABELING))
			rsp = smack_ipv6host_label(sip);
		if (rsp != NULL) {
			struct socket_smack *ssp = sock->sk->sk_security;

			rc = smk_ipv6_check(ssp->smk_out, rsp, sip,
					    SMK_CONNECTING);
		}
		if (__is_defined(SMACK_IPV6_PORT_LABELING))
			rc = smk_ipv6_port_check(sock->sk, sip, SMK_CONNECTING);

		return rc;
	}
	if (sap->sa_family != AF_INET || addrlen < sizeof(struct sockaddr_in))
		return 0;
	rc = smk_ipv4_check(sock->sk, (struct sockaddr_in *)sap);
	return rc;
}