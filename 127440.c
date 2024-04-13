static inline pte_t __pte(pteval_t val)
{
	pteval_t ret;

	if (sizeof(pteval_t) > sizeof(long))
		ret = PVOP_CALLEE2(pteval_t, mmu.make_pte, val, (u64)val >> 32);
	else
		ret = PVOP_CALLEE1(pteval_t, mmu.make_pte, val);

	return (pte_t) { .pte = ret };
}