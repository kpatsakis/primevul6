static inline void pte_clear(struct mm_struct *mm, unsigned long addr,
			     pte_t *ptep)
{
	PVOP_VCALL3(mmu.pte_clear, mm, addr, ptep);
}