static bool tipc_sk_connected(struct sock *sk)
{
	return sk->sk_state == TIPC_ESTABLISHED;
}