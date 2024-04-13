static int smack_socket_post_create(struct socket *sock, int family,
				    int type, int protocol, int kern)
{
	struct socket_smack *ssp;

	if (sock->sk == NULL)
		return 0;

	/*
	 * Sockets created by kernel threads receive web label.
	 */
	if (unlikely(current->flags & PF_KTHREAD)) {
		ssp = sock->sk->sk_security;
		ssp->smk_in = &smack_known_web;
		ssp->smk_out = &smack_known_web;
	}

	if (family != PF_INET)
		return 0;
	/*
	 * Set the outbound netlbl.
	 */
	return smack_netlbl_add(sock->sk);
}