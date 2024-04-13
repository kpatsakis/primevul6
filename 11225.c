static void tipc_sk_proto_rcv(struct sock *sk,
			      struct sk_buff_head *inputq,
			      struct sk_buff_head *xmitq)
{
	struct sk_buff *skb = __skb_dequeue(inputq);
	struct tipc_sock *tsk = tipc_sk(sk);
	struct tipc_msg *hdr = buf_msg(skb);
	struct tipc_group *grp = tsk->group;
	bool wakeup = false;

	switch (msg_user(hdr)) {
	case CONN_MANAGER:
		tipc_sk_conn_proto_rcv(tsk, skb, inputq, xmitq);
		return;
	case SOCK_WAKEUP:
		tipc_dest_del(&tsk->cong_links, msg_orignode(hdr), 0);
		/* coupled with smp_rmb() in tipc_wait_for_cond() */
		smp_wmb();
		tsk->cong_link_cnt--;
		wakeup = true;
		tipc_sk_push_backlog(tsk, false);
		break;
	case GROUP_PROTOCOL:
		tipc_group_proto_rcv(grp, &wakeup, hdr, inputq, xmitq);
		break;
	case TOP_SRV:
		tipc_group_member_evt(tsk->group, &wakeup, &sk->sk_rcvbuf,
				      hdr, inputq, xmitq);
		break;
	default:
		break;
	}

	if (wakeup)
		sk->sk_write_space(sk);

	kfree_skb(skb);
}