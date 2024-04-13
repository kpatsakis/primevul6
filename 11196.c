static int __tipc_bind(struct socket *sock, struct sockaddr *skaddr, int alen)
{
	struct tipc_uaddr *ua = (struct tipc_uaddr *)skaddr;
	struct tipc_sock *tsk = tipc_sk(sock->sk);
	bool unbind = false;

	if (unlikely(!alen))
		return tipc_sk_withdraw(tsk, NULL);

	if (ua->addrtype == TIPC_SERVICE_ADDR) {
		ua->addrtype = TIPC_SERVICE_RANGE;
		ua->sr.upper = ua->sr.lower;
	}
	if (ua->scope < 0) {
		unbind = true;
		ua->scope = -ua->scope;
	}
	/* Users may still use deprecated TIPC_ZONE_SCOPE */
	if (ua->scope != TIPC_NODE_SCOPE)
		ua->scope = TIPC_CLUSTER_SCOPE;

	if (tsk->group)
		return -EACCES;

	if (unbind)
		return tipc_sk_withdraw(tsk, ua);
	return tipc_sk_publish(tsk, ua);
}