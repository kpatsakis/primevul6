static inline void switch_fpu_finish(struct task_struct *new, fpu_switch_t fpu)
{
	if (fpu.preload) {
		if (unlikely(restore_fpu_checking(new)))
			drop_init_fpu(new);
	}
}