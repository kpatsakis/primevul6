static int tipc_send_group_unicast(struct socket *sock, struct msghdr *m,
				   int dlen, long timeout)
{
	struct sock *sk = sock->sk;
	struct tipc_uaddr *ua = (struct tipc_uaddr *)m->msg_name;
	int blks = tsk_blocks(GROUP_H_SIZE + dlen);
	struct tipc_sock *tsk = tipc_sk(sk);
	struct net *net = sock_net(sk);
	struct tipc_member *mb = NULL;
	u32 node, port;
	int rc;

	node = ua->sk.node;
	port = ua->sk.ref;
	if (!port && !node)
		return -EHOSTUNREACH;

	/* Block or return if destination link or member is congested */
	rc = tipc_wait_for_cond(sock, &timeout,
				!tipc_dest_find(&tsk->cong_links, node, 0) &&
				tsk->group &&
				!tipc_group_cong(tsk->group, node, port, blks,
						 &mb));
	if (unlikely(rc))
		return rc;

	if (unlikely(!mb))
		return -EHOSTUNREACH;

	rc = tipc_send_group_msg(net, tsk, m, mb, node, port, dlen);

	return rc ? rc : dlen;
}