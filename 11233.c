u32 tipc_sock_get_portid(struct sock *sk)
{
	return (sk) ? (tipc_sk(sk))->portid : 0;
}