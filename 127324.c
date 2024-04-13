static inline unsigned long __read_cr3(void)
{
	return PVOP_CALL0(unsigned long, mmu.read_cr3);
}