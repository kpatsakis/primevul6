pte_t *huge_pte_offset(struct mm_struct *mm,
		       unsigned long addr, unsigned long sz)
{
	pgd_t *pgd;
	p4d_t *p4d;
	pud_t *pud;
	pmd_t *pmd;

	pgd = pgd_offset(mm, addr);
	if (!pgd_present(*pgd))
		return NULL;
	p4d = p4d_offset(pgd, addr);
	if (!p4d_present(*p4d))
		return NULL;

	pud = pud_offset(p4d, addr);
	if (sz == PUD_SIZE)
		/* must be pud huge, non-present or none */
		return (pte_t *)pud;
	if (!pud_present(*pud))
		return NULL;
	/* must have a valid entry and size to go further */

	pmd = pmd_offset(pud, addr);
	/* must be pmd huge, non-present or none */
	return (pte_t *)pmd;
}