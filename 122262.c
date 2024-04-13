static inline void native_pte_clear(struct mm_struct *mm, unsigned long addr,
				    pte_t *ptep)
{
	ptep->pte_low = 0;
	smp_wmb();
	ptep->pte_high = 0;
}