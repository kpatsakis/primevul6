static inline int __thread_has_fpu(struct task_struct *tsk)
{
	return tsk->thread.fpu.has_fpu;
}