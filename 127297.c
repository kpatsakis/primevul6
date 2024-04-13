static inline notrace unsigned long arch_local_save_flags(void)
{
	return PVOP_CALLEE0(unsigned long, irq.save_fl);
}