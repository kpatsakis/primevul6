static enum audit_state audit_filter_task(struct task_struct *tsk)
{
	struct audit_entry *e;
	enum audit_state   state;

	rcu_read_lock();
	list_for_each_entry_rcu(e, &audit_filter_list[AUDIT_FILTER_TASK], list) {
		if (audit_filter_rules(tsk, &e->rule, NULL, NULL, &state)) {
			rcu_read_unlock();
			return state;
		}
	}
	rcu_read_unlock();
	return AUDIT_BUILD_CONTEXT;
}