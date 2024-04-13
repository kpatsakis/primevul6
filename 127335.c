static inline void paravirt_alloc_pmd(struct mm_struct *mm, unsigned long pfn)
{
	PVOP_VCALL2(mmu.alloc_pmd, mm, pfn);
}