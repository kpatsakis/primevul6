static inline pmd_t pmdp_get_and_clear(struct mm_struct *mm,
				       unsigned long address,
				       pmd_t *pmdp)
{
	pmd_t pmd = *pmdp;
	pmd_clear(mm, address, pmdp);
	return pmd;
}