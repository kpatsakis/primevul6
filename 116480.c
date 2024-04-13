static inline void __drop_fpu(struct task_struct *tsk)
{
	if (__thread_has_fpu(tsk)) {
		/* Ignore delayed exceptions from user space */
		asm volatile("1: fwait\n"
			     "2:\n"
			     _ASM_EXTABLE(1b, 2b));
		__thread_fpu_end(tsk);
	}
}