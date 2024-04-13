static inline u32 task_sid_subj(const struct task_struct *task)
{
	u32 sid;

	rcu_read_lock();
	sid = cred_sid(rcu_dereference(task->cred));
	rcu_read_unlock();
	return sid;
}