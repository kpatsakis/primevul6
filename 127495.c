static inline void arch_safe_halt(void)
{
	PVOP_VCALL0(irq.safe_halt);
}