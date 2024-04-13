static int tipc_send_group_bcast(struct socket *sock, struct msghdr *m,
				 int dlen, long timeout)
{
	struct tipc_uaddr *ua = (struct tipc_uaddr *)m->msg_name;
	struct sock *sk = sock->sk;
	struct net *net = sock_net(sk);
	struct tipc_sock *tsk = tipc_sk(sk);
	struct tipc_nlist *dsts;
	struct tipc_mc_method *method = &tsk->mc_method;
	bool ack = method->mandatory && method->rcast;
	int blks = tsk_blocks(MCAST_H_SIZE + dlen);
	struct tipc_msg *hdr = &tsk->phdr;
	int mtu = tipc_bcast_get_mtu(net);
	struct sk_buff_head pkts;
	int rc = -EHOSTUNREACH;

	/* Block or return if any destination link or member is congested */
	rc = tipc_wait_for_cond(sock, &timeout,
				!tsk->cong_link_cnt && tsk->group &&
				!tipc_group_bc_cong(tsk->group, blks));
	if (unlikely(rc))
		return rc;

	dsts = tipc_group_dests(tsk->group);
	if (!dsts->local && !dsts->remote)
		return -EHOSTUNREACH;

	/* Complete message header */
	if (ua) {
		msg_set_type(hdr, TIPC_GRP_MCAST_MSG);
		msg_set_nameinst(hdr, ua->sa.instance);
	} else {
		msg_set_type(hdr, TIPC_GRP_BCAST_MSG);
		msg_set_nameinst(hdr, 0);
	}
	msg_set_hdr_sz(hdr, GROUP_H_SIZE);
	msg_set_destport(hdr, 0);
	msg_set_destnode(hdr, 0);
	msg_set_grp_bc_seqno(hdr, tipc_group_bc_snd_nxt(tsk->group));

	/* Avoid getting stuck with repeated forced replicasts */
	msg_set_grp_bc_ack_req(hdr, ack);

	/* Build message as chain of buffers */
	__skb_queue_head_init(&pkts);
	rc = tipc_msg_build(hdr, m, 0, dlen, mtu, &pkts);
	if (unlikely(rc != dlen))
		return rc;

	/* Send message */
	rc = tipc_mcast_xmit(net, &pkts, method, dsts, &tsk->cong_link_cnt);
	if (unlikely(rc))
		return rc;

	/* Update broadcast sequence number and send windows */
	tipc_group_update_bc_members(tsk->group, blks, ack);

	/* Broadcast link is now free to choose method for next broadcast */
	method->mandatory = false;
	method->expires = jiffies;

	return dlen;
}