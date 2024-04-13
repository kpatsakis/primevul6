static int __tipc_nl_list_sk_publ(struct sk_buff *skb,
				  struct netlink_callback *cb,
				  struct tipc_sock *tsk, u32 *last_publ)
{
	int err;
	struct publication *p;

	if (*last_publ) {
		list_for_each_entry(p, &tsk->publications, binding_sock) {
			if (p->key == *last_publ)
				break;
		}
		if (p->key != *last_publ) {
			/* We never set seq or call nl_dump_check_consistent()
			 * this means that setting prev_seq here will cause the
			 * consistence check to fail in the netlink callback
			 * handler. Resulting in the last NLMSG_DONE message
			 * having the NLM_F_DUMP_INTR flag set.
			 */
			cb->prev_seq = 1;
			*last_publ = 0;
			return -EPIPE;
		}
	} else {
		p = list_first_entry(&tsk->publications, struct publication,
				     binding_sock);
	}

	list_for_each_entry_from(p, &tsk->publications, binding_sock) {
		err = __tipc_nl_add_sk_publ(skb, cb, p);
		if (err) {
			*last_publ = p->key;
			return err;
		}
	}
	*last_publ = 0;

	return 0;
}