static inline void paravirt_alloc_p4d(struct mm_struct *mm, unsigned long pfn)
{
	PVOP_VCALL2(mmu.alloc_p4d, mm, pfn);
}