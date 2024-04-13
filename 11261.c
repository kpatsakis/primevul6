int tipc_sk_fill_sock_diag(struct sk_buff *skb, struct netlink_callback *cb,
			   struct tipc_sock *tsk, u32 sk_filter_state,
			   u64 (*tipc_diag_gen_cookie)(struct sock *sk))
{
	struct sock *sk = &tsk->sk;
	struct nlattr *attrs;
	struct nlattr *stat;

	/*filter response w.r.t sk_state*/
	if (!(sk_filter_state & (1 << sk->sk_state)))
		return 0;

	attrs = nla_nest_start_noflag(skb, TIPC_NLA_SOCK);
	if (!attrs)
		goto msg_cancel;

	if (__tipc_nl_add_sk_info(skb, tsk))
		goto attr_msg_cancel;

	if (nla_put_u32(skb, TIPC_NLA_SOCK_TYPE, (u32)sk->sk_type) ||
	    nla_put_u32(skb, TIPC_NLA_SOCK_TIPC_STATE, (u32)sk->sk_state) ||
	    nla_put_u32(skb, TIPC_NLA_SOCK_INO, sock_i_ino(sk)) ||
	    nla_put_u32(skb, TIPC_NLA_SOCK_UID,
			from_kuid_munged(sk_user_ns(NETLINK_CB(cb->skb).sk),
					 sock_i_uid(sk))) ||
	    nla_put_u64_64bit(skb, TIPC_NLA_SOCK_COOKIE,
			      tipc_diag_gen_cookie(sk),
			      TIPC_NLA_SOCK_PAD))
		goto attr_msg_cancel;

	stat = nla_nest_start_noflag(skb, TIPC_NLA_SOCK_STAT);
	if (!stat)
		goto attr_msg_cancel;

	if (nla_put_u32(skb, TIPC_NLA_SOCK_STAT_RCVQ,
			skb_queue_len(&sk->sk_receive_queue)) ||
	    nla_put_u32(skb, TIPC_NLA_SOCK_STAT_SENDQ,
			skb_queue_len(&sk->sk_write_queue)) ||
	    nla_put_u32(skb, TIPC_NLA_SOCK_STAT_DROP,
			atomic_read(&sk->sk_drops)))
		goto stat_msg_cancel;

	if (tsk->cong_link_cnt &&
	    nla_put_flag(skb, TIPC_NLA_SOCK_STAT_LINK_CONG))
		goto stat_msg_cancel;

	if (tsk_conn_cong(tsk) &&
	    nla_put_flag(skb, TIPC_NLA_SOCK_STAT_CONN_CONG))
		goto stat_msg_cancel;

	nla_nest_end(skb, stat);

	if (tsk->group)
		if (tipc_group_fill_sock_diag(tsk->group, skb))
			goto stat_msg_cancel;

	nla_nest_end(skb, attrs);

	return 0;

stat_msg_cancel:
	nla_nest_cancel(skb, stat);
attr_msg_cancel:
	nla_nest_cancel(skb, attrs);
msg_cancel:
	return -EMSGSIZE;
}