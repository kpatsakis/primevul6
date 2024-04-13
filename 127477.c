static inline void write_cr3(unsigned long x)
{
	PVOP_VCALL1(mmu.write_cr3, x);
}