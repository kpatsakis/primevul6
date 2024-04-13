static inline void write_cr2(unsigned long x)
{
	PVOP_VCALL1(mmu.write_cr2, x);
}