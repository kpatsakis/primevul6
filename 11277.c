static int tipc_setsockopt(struct socket *sock, int lvl, int opt,
			   sockptr_t ov, unsigned int ol)
{
	struct sock *sk = sock->sk;
	struct tipc_sock *tsk = tipc_sk(sk);
	struct tipc_group_req mreq;
	u32 value = 0;
	int res = 0;

	if ((lvl == IPPROTO_TCP) && (sock->type == SOCK_STREAM))
		return 0;
	if (lvl != SOL_TIPC)
		return -ENOPROTOOPT;

	switch (opt) {
	case TIPC_IMPORTANCE:
	case TIPC_SRC_DROPPABLE:
	case TIPC_DEST_DROPPABLE:
	case TIPC_CONN_TIMEOUT:
	case TIPC_NODELAY:
		if (ol < sizeof(value))
			return -EINVAL;
		if (copy_from_sockptr(&value, ov, sizeof(u32)))
			return -EFAULT;
		break;
	case TIPC_GROUP_JOIN:
		if (ol < sizeof(mreq))
			return -EINVAL;
		if (copy_from_sockptr(&mreq, ov, sizeof(mreq)))
			return -EFAULT;
		break;
	default:
		if (!sockptr_is_null(ov) || ol)
			return -EINVAL;
	}

	lock_sock(sk);

	switch (opt) {
	case TIPC_IMPORTANCE:
		res = tsk_set_importance(sk, value);
		break;
	case TIPC_SRC_DROPPABLE:
		if (sock->type != SOCK_STREAM)
			tsk_set_unreliable(tsk, value);
		else
			res = -ENOPROTOOPT;
		break;
	case TIPC_DEST_DROPPABLE:
		tsk_set_unreturnable(tsk, value);
		break;
	case TIPC_CONN_TIMEOUT:
		tipc_sk(sk)->conn_timeout = value;
		break;
	case TIPC_MCAST_BROADCAST:
		tsk->mc_method.rcast = false;
		tsk->mc_method.mandatory = true;
		break;
	case TIPC_MCAST_REPLICAST:
		tsk->mc_method.rcast = true;
		tsk->mc_method.mandatory = true;
		break;
	case TIPC_GROUP_JOIN:
		res = tipc_sk_join(tsk, &mreq);
		break;
	case TIPC_GROUP_LEAVE:
		res = tipc_sk_leave(tsk);
		break;
	case TIPC_NODELAY:
		tsk->nodelay = !!value;
		tsk_set_nagle(tsk);
		break;
	default:
		res = -EINVAL;
	}

	release_sock(sk);

	return res;
}