static inline void paravirt_release_pmd(unsigned long pfn)
{
	PVOP_VCALL1(mmu.release_pmd, pfn);
}