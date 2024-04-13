static int tipc_sk_withdraw(struct tipc_sock *tsk, struct tipc_uaddr *ua)
{
	struct net *net = sock_net(&tsk->sk);
	struct publication *safe, *p;
	struct tipc_uaddr _ua;
	int rc = -EINVAL;

	list_for_each_entry_safe(p, safe, &tsk->publications, binding_sock) {
		if (!ua) {
			tipc_uaddr(&_ua, TIPC_SERVICE_RANGE, p->scope,
				   p->sr.type, p->sr.lower, p->sr.upper);
			tipc_nametbl_withdraw(net, &_ua, &p->sk, p->key);
			continue;
		}
		/* Unbind specific publication */
		if (p->scope != ua->scope)
			continue;
		if (p->sr.type != ua->sr.type)
			continue;
		if (p->sr.lower != ua->sr.lower)
			continue;
		if (p->sr.upper != ua->sr.upper)
			break;
		tipc_nametbl_withdraw(net, ua, &p->sk, p->key);
		rc = 0;
		break;
	}
	if (list_empty(&tsk->publications)) {
		tsk->published = 0;
		rc = 0;
	}
	return rc;
}