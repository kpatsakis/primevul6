static inline pudval_t pud_val(pud_t pud)
{
	return PVOP_CALLEE1(pudval_t, mmu.pud_val, pud.pud);
}