static int smack_socket_getpeersec_stream(struct socket *sock,
					  char __user *optval,
					  int __user *optlen, unsigned len)
{
	struct socket_smack *ssp;
	char *rcp = "";
	int slen = 1;
	int rc = 0;

	ssp = sock->sk->sk_security;
	if (ssp->smk_packet != NULL) {
		rcp = ssp->smk_packet->smk_known;
		slen = strlen(rcp) + 1;
	}

	if (slen > len)
		rc = -ERANGE;
	else if (copy_to_user(optval, rcp, slen) != 0)
		rc = -EFAULT;

	if (put_user(slen, optlen) != 0)
		rc = -EFAULT;

	return rc;
}