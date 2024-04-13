static inline void arch_start_context_switch(struct task_struct *prev)
{
	PVOP_VCALL1(cpu.start_context_switch, prev);
}