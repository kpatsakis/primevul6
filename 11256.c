static int tipc_socketpair(struct socket *sock1, struct socket *sock2)
{
	struct tipc_sock *tsk2 = tipc_sk(sock2->sk);
	struct tipc_sock *tsk1 = tipc_sk(sock1->sk);
	u32 onode = tipc_own_addr(sock_net(sock1->sk));

	tsk1->peer.family = AF_TIPC;
	tsk1->peer.addrtype = TIPC_SOCKET_ADDR;
	tsk1->peer.scope = TIPC_NODE_SCOPE;
	tsk1->peer.addr.id.ref = tsk2->portid;
	tsk1->peer.addr.id.node = onode;
	tsk2->peer.family = AF_TIPC;
	tsk2->peer.addrtype = TIPC_SOCKET_ADDR;
	tsk2->peer.scope = TIPC_NODE_SCOPE;
	tsk2->peer.addr.id.ref = tsk1->portid;
	tsk2->peer.addr.id.node = onode;

	tipc_sk_finish_conn(tsk1, tsk2->portid, onode);
	tipc_sk_finish_conn(tsk2, tsk1->portid, onode);
	return 0;
}