static inline pmd_t __pmd(pmdval_t val)
{
	pmdval_t ret;

	if (sizeof(pmdval_t) > sizeof(long))
		ret = PVOP_CALLEE2(pmdval_t, mmu.make_pmd, val, (u64)val >> 32);
	else
		ret = PVOP_CALLEE1(pmdval_t, mmu.make_pmd, val);

	return (pmd_t) { ret };
}