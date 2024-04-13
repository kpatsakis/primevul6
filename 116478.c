static inline int __save_init_fpu(struct task_struct *tsk)
{
	return fpu_save_init(&tsk->thread.fpu);
}