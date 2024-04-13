static void smack_sock_graft(struct sock *sk, struct socket *parent)
{
	struct socket_smack *ssp;
	struct smack_known *skp = smk_of_current();

	if (sk == NULL ||
	    (sk->sk_family != PF_INET && sk->sk_family != PF_INET6))
		return;

	ssp = sk->sk_security;
	ssp->smk_in = skp;
	ssp->smk_out = skp;
	/* cssp->smk_packet is already set in smack_inet_csk_clone() */
}