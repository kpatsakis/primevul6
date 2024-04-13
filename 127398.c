static inline void set_pte(pte_t *ptep, pte_t pte)
{
	if (sizeof(pteval_t) > sizeof(long))
		PVOP_VCALL3(mmu.set_pte, ptep, pte.pte, (u64)pte.pte >> 32);
	else
		PVOP_VCALL2(mmu.set_pte, ptep, pte.pte);
}