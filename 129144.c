enum audit_state audit_filter_inodes(struct task_struct *tsk,
				     struct audit_context *ctx)
{
	int i;
	struct audit_entry *e;
	enum audit_state state;

	if (audit_pid && tsk->tgid == audit_pid)
		return AUDIT_DISABLED;

	rcu_read_lock();
	for (i = 0; i < ctx->name_count; i++) {
		int word = AUDIT_WORD(ctx->major);
		int bit  = AUDIT_BIT(ctx->major);
		struct audit_names *n = &ctx->names[i];
		int h = audit_hash_ino((u32)n->ino);
		struct list_head *list = &audit_inode_hash[h];

		if (list_empty(list))
			continue;

		list_for_each_entry_rcu(e, list, list) {
			if ((e->rule.mask[word] & bit) == bit &&
			    audit_filter_rules(tsk, &e->rule, ctx, n, &state)) {
				rcu_read_unlock();
				return state;
			}
		}
	}
	rcu_read_unlock();
	return AUDIT_BUILD_CONTEXT;
}