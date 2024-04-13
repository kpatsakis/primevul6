static inline void __thread_set_has_fpu(struct task_struct *tsk)
{
	tsk->thread.fpu.has_fpu = 1;
	this_cpu_write(fpu_owner_task, tsk);
}