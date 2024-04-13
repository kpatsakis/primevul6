static inline void paravirt_arch_dup_mmap(struct mm_struct *oldmm,
					  struct mm_struct *mm)
{
	PVOP_VCALL2(mmu.dup_mmap, oldmm, mm);
}