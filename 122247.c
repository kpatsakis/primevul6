static inline void __ptep_modify_prot_commit(struct mm_struct *mm,
					     unsigned long addr,
					     pte_t *ptep, pte_t pte)
{
	/*
	 * The pte is non-present, so there's no hardware state to
	 * preserve.
	 */
	set_pte_at(mm, addr, ptep, pte);
}