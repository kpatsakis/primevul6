static int smack_netlbl_add(struct sock *sk)
{
	struct socket_smack *ssp = sk->sk_security;
	struct smack_known *skp = ssp->smk_out;
	int rc;

	local_bh_disable();
	bh_lock_sock_nested(sk);

	rc = netlbl_sock_setattr(sk, sk->sk_family, &skp->smk_netlabel);
	switch (rc) {
	case 0:
		ssp->smk_state = SMK_NETLBL_LABELED;
		break;
	case -EDESTADDRREQ:
		ssp->smk_state = SMK_NETLBL_REQSKB;
		rc = 0;
		break;
	}

	bh_unlock_sock(sk);
	local_bh_enable();

	return rc;
}