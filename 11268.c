int tipc_nl_sk_walk(struct sk_buff *skb, struct netlink_callback *cb,
		    int (*skb_handler)(struct sk_buff *skb,
				       struct netlink_callback *cb,
				       struct tipc_sock *tsk))
{
	struct rhashtable_iter *iter = (void *)cb->args[4];
	struct tipc_sock *tsk;
	int err;

	rhashtable_walk_start(iter);
	while ((tsk = rhashtable_walk_next(iter)) != NULL) {
		if (IS_ERR(tsk)) {
			err = PTR_ERR(tsk);
			if (err == -EAGAIN) {
				err = 0;
				continue;
			}
			break;
		}

		sock_hold(&tsk->sk);
		rhashtable_walk_stop(iter);
		lock_sock(&tsk->sk);
		err = skb_handler(skb, cb, tsk);
		if (err) {
			release_sock(&tsk->sk);
			sock_put(&tsk->sk);
			goto out;
		}
		release_sock(&tsk->sk);
		rhashtable_walk_start(iter);
		sock_put(&tsk->sk);
	}
	rhashtable_walk_stop(iter);
out:
	return skb->len;
}