static inline void __set_pgd(pgd_t *pgdp, pgd_t pgd)
{
	PVOP_VCALL2(mmu.set_pgd, pgdp, native_pgd_val(pgd));
}