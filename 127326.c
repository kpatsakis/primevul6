static inline void paravirt_release_pte(unsigned long pfn)
{
	PVOP_VCALL1(mmu.release_pte, pfn);
}