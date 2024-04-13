static int smack_socket_socketpair(struct socket *socka,
		                   struct socket *sockb)
{
	struct socket_smack *asp = socka->sk->sk_security;
	struct socket_smack *bsp = sockb->sk->sk_security;

	asp->smk_packet = bsp->smk_out;
	bsp->smk_packet = asp->smk_out;

	return 0;
}