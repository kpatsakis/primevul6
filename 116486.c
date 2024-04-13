static inline void __thread_fpu_begin(struct task_struct *tsk)
{
	if (!static_cpu_has_safe(X86_FEATURE_EAGER_FPU))
		clts();
	__thread_set_has_fpu(tsk);
}