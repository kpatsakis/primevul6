static int tipc_send_group_anycast(struct socket *sock, struct msghdr *m,
				   int dlen, long timeout)
{
	struct tipc_uaddr *ua = (struct tipc_uaddr *)m->msg_name;
	struct sock *sk = sock->sk;
	struct tipc_sock *tsk = tipc_sk(sk);
	struct list_head *cong_links = &tsk->cong_links;
	int blks = tsk_blocks(GROUP_H_SIZE + dlen);
	struct tipc_msg *hdr = &tsk->phdr;
	struct tipc_member *first = NULL;
	struct tipc_member *mbr = NULL;
	struct net *net = sock_net(sk);
	u32 node, port, exclude;
	struct list_head dsts;
	int lookups = 0;
	int dstcnt, rc;
	bool cong;

	INIT_LIST_HEAD(&dsts);
	ua->sa.type = msg_nametype(hdr);
	ua->scope = msg_lookup_scope(hdr);

	while (++lookups < 4) {
		exclude = tipc_group_exclude(tsk->group);

		first = NULL;

		/* Look for a non-congested destination member, if any */
		while (1) {
			if (!tipc_nametbl_lookup_group(net, ua, &dsts, &dstcnt,
						       exclude, false))
				return -EHOSTUNREACH;
			tipc_dest_pop(&dsts, &node, &port);
			cong = tipc_group_cong(tsk->group, node, port, blks,
					       &mbr);
			if (!cong)
				break;
			if (mbr == first)
				break;
			if (!first)
				first = mbr;
		}

		/* Start over if destination was not in member list */
		if (unlikely(!mbr))
			continue;

		if (likely(!cong && !tipc_dest_find(cong_links, node, 0)))
			break;

		/* Block or return if destination link or member is congested */
		rc = tipc_wait_for_cond(sock, &timeout,
					!tipc_dest_find(cong_links, node, 0) &&
					tsk->group &&
					!tipc_group_cong(tsk->group, node, port,
							 blks, &mbr));
		if (unlikely(rc))
			return rc;

		/* Send, unless destination disappeared while waiting */
		if (likely(mbr))
			break;
	}

	if (unlikely(lookups >= 4))
		return -EHOSTUNREACH;

	rc = tipc_send_group_msg(net, tsk, m, mbr, node, port, dlen);

	return rc ? rc : dlen;
}