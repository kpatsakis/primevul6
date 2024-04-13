static int tipc_sk_join(struct tipc_sock *tsk, struct tipc_group_req *mreq)
{
	struct net *net = sock_net(&tsk->sk);
	struct tipc_group *grp = tsk->group;
	struct tipc_msg *hdr = &tsk->phdr;
	struct tipc_uaddr ua;
	int rc;

	if (mreq->type < TIPC_RESERVED_TYPES)
		return -EACCES;
	if (mreq->scope > TIPC_NODE_SCOPE)
		return -EINVAL;
	if (mreq->scope != TIPC_NODE_SCOPE)
		mreq->scope = TIPC_CLUSTER_SCOPE;
	if (grp)
		return -EACCES;
	grp = tipc_group_create(net, tsk->portid, mreq, &tsk->group_is_open);
	if (!grp)
		return -ENOMEM;
	tsk->group = grp;
	msg_set_lookup_scope(hdr, mreq->scope);
	msg_set_nametype(hdr, mreq->type);
	msg_set_dest_droppable(hdr, true);
	tipc_uaddr(&ua, TIPC_SERVICE_RANGE, mreq->scope,
		   mreq->type, mreq->instance, mreq->instance);
	tipc_nametbl_build_group(net, grp, &ua);
	rc = tipc_sk_publish(tsk, &ua);
	if (rc) {
		tipc_group_delete(net, grp);
		tsk->group = NULL;
		return rc;
	}
	/* Eliminate any risk that a broadcast overtakes sent JOINs */
	tsk->mc_method.rcast = true;
	tsk->mc_method.mandatory = true;
	tipc_group_join(net, grp, &tsk->sk.sk_rcvbuf);
	return rc;
}