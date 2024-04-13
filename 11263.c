static bool tipc_sk_type_connectionless(struct sock *sk)
{
	return sk->sk_type == SOCK_RDM || sk->sk_type == SOCK_DGRAM;
}