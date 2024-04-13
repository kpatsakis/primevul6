static inline void arch_end_context_switch(struct task_struct *next)
{
	PVOP_VCALL1(cpu.end_context_switch, next);
}