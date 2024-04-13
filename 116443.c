static inline void __cpu_disable_lazy_restore(unsigned int cpu)
{
	per_cpu(fpu_owner_task, cpu) = NULL;
}