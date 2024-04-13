static inline pud_t __pud(pudval_t val)
{
	pudval_t ret;

	ret = PVOP_CALLEE1(pudval_t, mmu.make_pud, val);

	return (pud_t) { ret };
}