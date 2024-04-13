int tipc_sk_bind(struct socket *sock, struct sockaddr *skaddr, int alen)
{
	int res;

	lock_sock(sock->sk);
	res = __tipc_bind(sock, skaddr, alen);
	release_sock(sock->sk);
	return res;
}