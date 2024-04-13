static inline void drop_fpu(struct task_struct *tsk)
{
	/*
	 * Forget coprocessor state..
	 */
	preempt_disable();
	tsk->thread.fpu_counter = 0;
	__drop_fpu(tsk);
	clear_used_math();
	preempt_enable();
}