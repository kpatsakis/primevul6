static inline void halt(void)
{
	PVOP_VCALL0(irq.halt);
}