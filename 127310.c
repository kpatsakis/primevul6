static inline void set_pte_at(struct mm_struct *mm, unsigned long addr,
			      pte_t *ptep, pte_t pte)
{
	if (sizeof(pteval_t) > sizeof(long))
		/* 5 arg words */
		pv_ops.mmu.set_pte_at(mm, addr, ptep, pte);
	else
		PVOP_VCALL4(mmu.set_pte_at, mm, addr, ptep, pte.pte);
}