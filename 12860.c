static int smack_socket_sendmsg(struct socket *sock, struct msghdr *msg,
				int size)
{
	struct sockaddr_in *sip = (struct sockaddr_in *) msg->msg_name;
#if IS_ENABLED(CONFIG_IPV6)
	struct sockaddr_in6 *sap = (struct sockaddr_in6 *) msg->msg_name;
#endif
#ifdef SMACK_IPV6_SECMARK_LABELING
	struct socket_smack *ssp = sock->sk->sk_security;
	struct smack_known *rsp;
#endif
	int rc = 0;

	/*
	 * Perfectly reasonable for this to be NULL
	 */
	if (sip == NULL)
		return 0;

	switch (sock->sk->sk_family) {
	case AF_INET:
		if (msg->msg_namelen < sizeof(struct sockaddr_in) ||
		    sip->sin_family != AF_INET)
			return -EINVAL;
		rc = smk_ipv4_check(sock->sk, sip);
		break;
#if IS_ENABLED(CONFIG_IPV6)
	case AF_INET6:
		if (msg->msg_namelen < SIN6_LEN_RFC2133 ||
		    sap->sin6_family != AF_INET6)
			return -EINVAL;
#ifdef SMACK_IPV6_SECMARK_LABELING
		rsp = smack_ipv6host_label(sap);
		if (rsp != NULL)
			rc = smk_ipv6_check(ssp->smk_out, rsp, sap,
						SMK_CONNECTING);
#endif
#ifdef SMACK_IPV6_PORT_LABELING
		rc = smk_ipv6_port_check(sock->sk, sap, SMK_SENDING);
#endif
#endif /* IS_ENABLED(CONFIG_IPV6) */
		break;
	}
	return rc;
}