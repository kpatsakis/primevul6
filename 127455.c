static inline notrace unsigned long arch_local_irq_save(void)
{
	unsigned long f;

	f = arch_local_save_flags();
	arch_local_irq_disable();
	return f;
}