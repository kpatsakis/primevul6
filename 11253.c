void tipc_sk_reinit(struct net *net)
{
	struct tipc_net *tn = net_generic(net, tipc_net_id);
	struct rhashtable_iter iter;
	struct tipc_sock *tsk;
	struct tipc_msg *msg;

	rhashtable_walk_enter(&tn->sk_rht, &iter);

	do {
		rhashtable_walk_start(&iter);

		while ((tsk = rhashtable_walk_next(&iter)) && !IS_ERR(tsk)) {
			sock_hold(&tsk->sk);
			rhashtable_walk_stop(&iter);
			lock_sock(&tsk->sk);
			msg = &tsk->phdr;
			msg_set_prevnode(msg, tipc_own_addr(net));
			msg_set_orignode(msg, tipc_own_addr(net));
			release_sock(&tsk->sk);
			rhashtable_walk_start(&iter);
			sock_put(&tsk->sk);
		}

		rhashtable_walk_stop(&iter);
	} while (tsk == ERR_PTR(-EAGAIN));

	rhashtable_walk_exit(&iter);
}