static inline void paravirt_alloc_pte(struct mm_struct *mm, unsigned long pfn)
{
	PVOP_VCALL2(mmu.alloc_pte, mm, pfn);
}