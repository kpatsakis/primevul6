static inline fpu_switch_t switch_fpu_prepare(struct task_struct *old, struct task_struct *new, int cpu)
{
	fpu_switch_t fpu;

	/*
	 * If the task has used the math, pre-load the FPU on xsave processors
	 * or if the past 5 consecutive context-switches used math.
	 */
	fpu.preload = tsk_used_math(new) && (use_eager_fpu() ||
					     new->thread.fpu_counter > 5);
	if (__thread_has_fpu(old)) {
		if (!__save_init_fpu(old))
			cpu = ~0;
		old->thread.fpu.last_cpu = cpu;
		old->thread.fpu.has_fpu = 0;	/* But leave fpu_owner_task! */

		/* Don't change CR0.TS if we just switch! */
		if (fpu.preload) {
			new->thread.fpu_counter++;
			__thread_set_has_fpu(new);
			prefetch(new->thread.fpu.state);
		} else if (!use_eager_fpu())
			stts();
	} else {
		old->thread.fpu_counter = 0;
		old->thread.fpu.last_cpu = ~0;
		if (fpu.preload) {
			new->thread.fpu_counter++;
			if (!use_eager_fpu() && fpu_lazy_restore(new, cpu))
				fpu.preload = 0;
			else
				prefetch(new->thread.fpu.state);
			__thread_fpu_begin(new);
		}
	}
	return fpu;
}