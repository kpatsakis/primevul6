static int smk_ipv6_port_check(struct sock *sk, struct sockaddr_in6 *address,
				int act)
{
	struct smk_port_label *spp;
	struct socket_smack *ssp = sk->sk_security;
	struct smack_known *skp = NULL;
	unsigned short port;
	struct smack_known *object;

	if (act == SMK_RECEIVING) {
		skp = smack_ipv6host_label(address);
		object = ssp->smk_in;
	} else {
		skp = ssp->smk_out;
		object = smack_ipv6host_label(address);
	}

	/*
	 * The other end is a single label host.
	 */
	if (skp != NULL && object != NULL)
		return smk_ipv6_check(skp, object, address, act);
	if (skp == NULL)
		skp = smack_net_ambient;
	if (object == NULL)
		object = smack_net_ambient;

	/*
	 * It's remote, so port lookup does no good.
	 */
	if (!smk_ipv6_localhost(address))
		return smk_ipv6_check(skp, object, address, act);

	/*
	 * It's local so the send check has to have passed.
	 */
	if (act == SMK_RECEIVING)
		return 0;

	port = ntohs(address->sin6_port);
	rcu_read_lock();
	list_for_each_entry_rcu(spp, &smk_ipv6_port_list, list) {
		if (spp->smk_port != port || spp->smk_sock_type != sk->sk_type)
			continue;
		object = spp->smk_in;
		if (act == SMK_CONNECTING)
			ssp->smk_packet = spp->smk_out;
		break;
	}
	rcu_read_unlock();

	return smk_ipv6_check(skp, object, address, act);
}