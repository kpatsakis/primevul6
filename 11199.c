static int tipc_send_group_mcast(struct socket *sock, struct msghdr *m,
				 int dlen, long timeout)
{
	struct tipc_uaddr *ua = (struct tipc_uaddr *)m->msg_name;
	struct sock *sk = sock->sk;
	struct tipc_sock *tsk = tipc_sk(sk);
	struct tipc_group *grp = tsk->group;
	struct tipc_msg *hdr = &tsk->phdr;
	struct net *net = sock_net(sk);
	struct list_head dsts;
	u32 dstcnt, exclude;

	INIT_LIST_HEAD(&dsts);
	ua->sa.type = msg_nametype(hdr);
	ua->scope = msg_lookup_scope(hdr);
	exclude = tipc_group_exclude(grp);

	if (!tipc_nametbl_lookup_group(net, ua, &dsts, &dstcnt, exclude, true))
		return -EHOSTUNREACH;

	if (dstcnt == 1) {
		tipc_dest_pop(&dsts, &ua->sk.node, &ua->sk.ref);
		return tipc_send_group_unicast(sock, m, dlen, timeout);
	}

	tipc_dest_list_purge(&dsts);
	return tipc_send_group_bcast(sock, m, dlen, timeout);
}