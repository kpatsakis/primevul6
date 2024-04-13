static int tipc_sendmcast(struct  socket *sock, struct tipc_uaddr *ua,
			  struct msghdr *msg, size_t dlen, long timeout)
{
	struct sock *sk = sock->sk;
	struct tipc_sock *tsk = tipc_sk(sk);
	struct tipc_msg *hdr = &tsk->phdr;
	struct net *net = sock_net(sk);
	int mtu = tipc_bcast_get_mtu(net);
	struct sk_buff_head pkts;
	struct tipc_nlist dsts;
	int rc;

	if (tsk->group)
		return -EACCES;

	/* Block or return if any destination link is congested */
	rc = tipc_wait_for_cond(sock, &timeout, !tsk->cong_link_cnt);
	if (unlikely(rc))
		return rc;

	/* Lookup destination nodes */
	tipc_nlist_init(&dsts, tipc_own_addr(net));
	tipc_nametbl_lookup_mcast_nodes(net, ua, &dsts);
	if (!dsts.local && !dsts.remote)
		return -EHOSTUNREACH;

	/* Build message header */
	msg_set_type(hdr, TIPC_MCAST_MSG);
	msg_set_hdr_sz(hdr, MCAST_H_SIZE);
	msg_set_lookup_scope(hdr, TIPC_CLUSTER_SCOPE);
	msg_set_destport(hdr, 0);
	msg_set_destnode(hdr, 0);
	msg_set_nametype(hdr, ua->sr.type);
	msg_set_namelower(hdr, ua->sr.lower);
	msg_set_nameupper(hdr, ua->sr.upper);

	/* Build message as chain of buffers */
	__skb_queue_head_init(&pkts);
	rc = tipc_msg_build(hdr, msg, 0, dlen, mtu, &pkts);

	/* Send message if build was successful */
	if (unlikely(rc == dlen)) {
		trace_tipc_sk_sendmcast(sk, skb_peek(&pkts),
					TIPC_DUMP_SK_SNDQ, " ");
		rc = tipc_mcast_xmit(net, &pkts, &tsk->mc_method, &dsts,
				     &tsk->cong_link_cnt);
	}

	tipc_nlist_purge(&dsts);

	return rc ? rc : dlen;
}