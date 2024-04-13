static inline void paravirt_release_pud(unsigned long pfn)
{
	PVOP_VCALL1(mmu.release_pud, pfn);
}