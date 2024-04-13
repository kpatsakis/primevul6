static inline void pte_clear(struct mm_struct *mm, unsigned long addr,
			     pte_t *ptep)
{
	set_pte_at(mm, addr, ptep, __pte(0));
}