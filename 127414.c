static inline notrace void arch_local_irq_enable(void)
{
	PVOP_VCALLEE0(irq.irq_enable);
}