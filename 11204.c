static int tipc_send_group_msg(struct net *net, struct tipc_sock *tsk,
			       struct msghdr *m, struct tipc_member *mb,
			       u32 dnode, u32 dport, int dlen)
{
	u16 bc_snd_nxt = tipc_group_bc_snd_nxt(tsk->group);
	struct tipc_mc_method *method = &tsk->mc_method;
	int blks = tsk_blocks(GROUP_H_SIZE + dlen);
	struct tipc_msg *hdr = &tsk->phdr;
	struct sk_buff_head pkts;
	int mtu, rc;

	/* Complete message header */
	msg_set_type(hdr, TIPC_GRP_UCAST_MSG);
	msg_set_hdr_sz(hdr, GROUP_H_SIZE);
	msg_set_destport(hdr, dport);
	msg_set_destnode(hdr, dnode);
	msg_set_grp_bc_seqno(hdr, bc_snd_nxt);

	/* Build message as chain of buffers */
	__skb_queue_head_init(&pkts);
	mtu = tipc_node_get_mtu(net, dnode, tsk->portid, false);
	rc = tipc_msg_build(hdr, m, 0, dlen, mtu, &pkts);
	if (unlikely(rc != dlen))
		return rc;

	/* Send message */
	rc = tipc_node_xmit(net, &pkts, dnode, tsk->portid);
	if (unlikely(rc == -ELINKCONG)) {
		tipc_dest_push(&tsk->cong_links, dnode, 0);
		tsk->cong_link_cnt++;
	}

	/* Update send window */
	tipc_group_update_member(mb, blks);

	/* A broadcast sent within next EXPIRE period must follow same path */
	method->rcast = true;
	method->mandatory = true;
	return dlen;
}