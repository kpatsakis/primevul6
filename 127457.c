static inline void set_pte_atomic(pte_t *ptep, pte_t pte)
{
	PVOP_VCALL3(mmu.set_pte_atomic, ptep, pte.pte, pte.pte >> 32);
}