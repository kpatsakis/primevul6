static inline void __thread_clear_has_fpu(struct task_struct *tsk)
{
	tsk->thread.fpu.has_fpu = 0;
	this_cpu_write(fpu_owner_task, NULL);
}