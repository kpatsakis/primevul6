static inline unsigned long read_cr2(void)
{
	return PVOP_CALLEE0(unsigned long, mmu.read_cr2);
}