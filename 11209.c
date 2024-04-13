int __tipc_dump_start(struct netlink_callback *cb, struct net *net)
{
	/* tipc_nl_name_table_dump() uses cb->args[0...3]. */
	struct rhashtable_iter *iter = (void *)cb->args[4];
	struct tipc_net *tn = tipc_net(net);

	if (!iter) {
		iter = kmalloc(sizeof(*iter), GFP_KERNEL);
		if (!iter)
			return -ENOMEM;

		cb->args[4] = (long)iter;
	}

	rhashtable_walk_enter(&tn->sk_rht, iter);
	return 0;
}