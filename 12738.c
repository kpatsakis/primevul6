static void smk_ipv6_port_label(struct socket *sock, struct sockaddr *address)
{
	struct sock *sk = sock->sk;
	struct sockaddr_in6 *addr6;
	struct socket_smack *ssp = sock->sk->sk_security;
	struct smk_port_label *spp;
	unsigned short port = 0;

	if (address == NULL) {
		/*
		 * This operation is changing the Smack information
		 * on the bound socket. Take the changes to the port
		 * as well.
		 */
		rcu_read_lock();
		list_for_each_entry_rcu(spp, &smk_ipv6_port_list, list) {
			if (sk != spp->smk_sock)
				continue;
			spp->smk_in = ssp->smk_in;
			spp->smk_out = ssp->smk_out;
			rcu_read_unlock();
			return;
		}
		/*
		 * A NULL address is only used for updating existing
		 * bound entries. If there isn't one, it's OK.
		 */
		rcu_read_unlock();
		return;
	}

	addr6 = (struct sockaddr_in6 *)address;
	port = ntohs(addr6->sin6_port);
	/*
	 * This is a special case that is safely ignored.
	 */
	if (port == 0)
		return;

	/*
	 * Look for an existing port list entry.
	 * This is an indication that a port is getting reused.
	 */
	rcu_read_lock();
	list_for_each_entry_rcu(spp, &smk_ipv6_port_list, list) {
		if (spp->smk_port != port || spp->smk_sock_type != sock->type)
			continue;
		if (spp->smk_can_reuse != 1) {
			rcu_read_unlock();
			return;
		}
		spp->smk_port = port;
		spp->smk_sock = sk;
		spp->smk_in = ssp->smk_in;
		spp->smk_out = ssp->smk_out;
		spp->smk_can_reuse = 0;
		rcu_read_unlock();
		return;
	}
	rcu_read_unlock();
	/*
	 * A new port entry is required.
	 */
	spp = kzalloc(sizeof(*spp), GFP_KERNEL);
	if (spp == NULL)
		return;

	spp->smk_port = port;
	spp->smk_sock = sk;
	spp->smk_in = ssp->smk_in;
	spp->smk_out = ssp->smk_out;
	spp->smk_sock_type = sock->type;
	spp->smk_can_reuse = 0;

	mutex_lock(&smack_ipv6_lock);
	list_add_rcu(&spp->list, &smk_ipv6_port_list);
	mutex_unlock(&smack_ipv6_lock);
	return;
}