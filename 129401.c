static void fib6_dump_end(struct netlink_callback *cb)
{
	struct fib6_walker_t *w = (void*)cb->args[2];

	if (w) {
		if (cb->args[4]) {
			cb->args[4] = 0;
			fib6_walker_unlink(w);
		}
		cb->args[2] = 0;
		kfree(w);
	}
	cb->done = (void*)cb->args[3];
	cb->args[1] = 3;
}