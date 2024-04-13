static inline void paravirt_alloc_pud(struct mm_struct *mm, unsigned long pfn)
{
	PVOP_VCALL2(mmu.alloc_pud, mm, pfn);
}