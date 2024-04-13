static inline pteval_t pte_val(pte_t pte)
{
	pteval_t ret;

	if (sizeof(pteval_t) > sizeof(long))
		ret = PVOP_CALLEE2(pteval_t, mmu.pte_val,
				   pte.pte, (u64)pte.pte >> 32);
	else
		ret = PVOP_CALLEE1(pteval_t, mmu.pte_val, pte.pte);

	return ret;
}