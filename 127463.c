static inline void paravirt_pgd_free(struct mm_struct *mm, pgd_t *pgd)
{
	PVOP_VCALL2(mmu.pgd_free, mm, pgd);
}