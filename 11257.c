static int tipc_getname(struct socket *sock, struct sockaddr *uaddr,
			int peer)
{
	struct sockaddr_tipc *addr = (struct sockaddr_tipc *)uaddr;
	struct sock *sk = sock->sk;
	struct tipc_sock *tsk = tipc_sk(sk);

	memset(addr, 0, sizeof(*addr));
	if (peer) {
		if ((!tipc_sk_connected(sk)) &&
		    ((peer != 2) || (sk->sk_state != TIPC_DISCONNECTING)))
			return -ENOTCONN;
		addr->addr.id.ref = tsk_peer_port(tsk);
		addr->addr.id.node = tsk_peer_node(tsk);
	} else {
		addr->addr.id.ref = tsk->portid;
		addr->addr.id.node = tipc_own_addr(sock_net(sk));
	}

	addr->addrtype = TIPC_SOCKET_ADDR;
	addr->family = AF_TIPC;
	addr->scope = 0;
	addr->addr.name.domain = 0;

	return sizeof(*addr);
}