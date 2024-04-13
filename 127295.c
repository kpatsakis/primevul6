static inline notrace void arch_local_irq_disable(void)
{
	PVOP_VCALLEE0(irq.irq_disable);
}