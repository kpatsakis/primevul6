static inline int fpu_lazy_restore(struct task_struct *new, unsigned int cpu)
{
	return new == this_cpu_read_stable(fpu_owner_task) &&
		cpu == new->thread.fpu.last_cpu;
}