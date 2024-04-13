static int tipc_sk_publish(struct tipc_sock *tsk, struct tipc_uaddr *ua)
{
	struct sock *sk = &tsk->sk;
	struct net *net = sock_net(sk);
	struct tipc_socket_addr skaddr;
	struct publication *p;
	u32 key;

	if (tipc_sk_connected(sk))
		return -EINVAL;
	key = tsk->portid + tsk->pub_count + 1;
	if (key == tsk->portid)
		return -EADDRINUSE;
	skaddr.ref = tsk->portid;
	skaddr.node = tipc_own_addr(net);
	p = tipc_nametbl_publish(net, ua, &skaddr, key);
	if (unlikely(!p))
		return -EINVAL;

	list_add(&p->binding_sock, &tsk->publications);
	tsk->pub_count++;
	tsk->published = true;
	return 0;
}