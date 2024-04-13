static inline void pmdp_set_wrprotect(struct mm_struct *mm,
				      unsigned long address, pmd_t *pmdp)
{
	pmd_t old_pmd = *pmdp;
	set_pmd_at(mm, address, pmdp, pmd_wrprotect(old_pmd));
}