static inline int zap_threads(struct task_struct *tsk, struct mm_struct *mm,
				int exit_code)
{
	struct task_struct *g, *p;
	unsigned long flags;
	int err = -EAGAIN;

	spin_lock_irq(&tsk->sighand->siglock);
	if (!(tsk->signal->flags & SIGNAL_GROUP_EXIT)) {
		tsk->signal->group_exit_code = exit_code;
		zap_process(tsk);
		err = 0;
	}
	spin_unlock_irq(&tsk->sighand->siglock);
	if (err)
		return err;

	if (atomic_read(&mm->mm_users) == mm->core_waiters + 1)
		goto done;

	rcu_read_lock();
	for_each_process(g) {
		if (g == tsk->group_leader)
			continue;

		p = g;
		do {
			if (p->mm) {
				if (p->mm == mm) {
					/*
					 * p->sighand can't disappear, but
					 * may be changed by de_thread()
					 */
					lock_task_sighand(p, &flags);
					zap_process(p);
					unlock_task_sighand(p, &flags);
				}
				break;
			}
		} while ((p = next_thread(p)) != g);
	}
	rcu_read_unlock();
done:
	return mm->core_waiters;
}