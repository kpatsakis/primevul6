static void smack_inet_csk_clone(struct sock *sk,
				 const struct request_sock *req)
{
	struct socket_smack *ssp = sk->sk_security;
	struct smack_known *skp;

	if (req->peer_secid != 0) {
		skp = smack_from_secid(req->peer_secid);
		ssp->smk_packet = skp;
	} else
		ssp->smk_packet = NULL;
}