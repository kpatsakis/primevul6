static inline int paravirt_pgd_alloc(struct mm_struct *mm)
{
	return PVOP_CALL1(int, mmu.pgd_alloc, mm);
}