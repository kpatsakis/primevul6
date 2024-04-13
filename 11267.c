static int tipc_sk_leave(struct tipc_sock *tsk)
{
	struct net *net = sock_net(&tsk->sk);
	struct tipc_group *grp = tsk->group;
	struct tipc_uaddr ua;
	int scope;

	if (!grp)
		return -EINVAL;
	ua.addrtype = TIPC_SERVICE_RANGE;
	tipc_group_self(grp, &ua.sr, &scope);
	ua.scope = scope;
	tipc_group_delete(net, grp);
	tsk->group = NULL;
	tipc_sk_withdraw(tsk, &ua);
	return 0;
}