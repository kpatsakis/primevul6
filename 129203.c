static void zap_process(struct task_struct *start)
{
	struct task_struct *t;

	start->signal->flags = SIGNAL_GROUP_EXIT;
	start->signal->group_stop_count = 0;

	t = start;
	do {
		if (t != current && t->mm) {
			t->mm->core_waiters++;
			sigaddset(&t->pending.signal, SIGKILL);
			signal_wake_up(t, 1);
		}
	} while ((t = next_thread(t)) != start);
}