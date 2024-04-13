static inline notrace void arch_local_irq_restore(unsigned long f)
{
	PVOP_VCALLEE1(irq.restore_fl, f);
}