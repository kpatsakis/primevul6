static int smack_sk_alloc_security(struct sock *sk, int family, gfp_t gfp_flags)
{
	struct smack_known *skp = smk_of_current();
	struct socket_smack *ssp;

	ssp = kzalloc(sizeof(struct socket_smack), gfp_flags);
	if (ssp == NULL)
		return -ENOMEM;

	/*
	 * Sockets created by kernel threads receive web label.
	 */
	if (unlikely(current->flags & PF_KTHREAD)) {
		ssp->smk_in = &smack_known_web;
		ssp->smk_out = &smack_known_web;
	} else {
		ssp->smk_in = skp;
		ssp->smk_out = skp;
	}
	ssp->smk_packet = NULL;

	sk->sk_security = ssp;

	return 0;
}