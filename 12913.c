static void smack_netlbl_delete(struct sock *sk)
{
	struct socket_smack *ssp = sk->sk_security;

	/*
	 * Take the label off the socket if one is set.
	 */
	if (ssp->smk_state != SMK_NETLBL_LABELED)
		return;

	local_bh_disable();
	bh_lock_sock_nested(sk);
	netlbl_sock_delattr(sk);
	bh_unlock_sock(sk);
	local_bh_enable();
	ssp->smk_state = SMK_NETLBL_UNLABELED;
}