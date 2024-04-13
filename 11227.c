int tipc_dump_done(struct netlink_callback *cb)
{
	struct rhashtable_iter *hti = (void *)cb->args[4];

	rhashtable_walk_exit(hti);
	kfree(hti);
	return 0;
}