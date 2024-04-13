follow_huge_pud(struct mm_struct *mm, unsigned long address,
		pud_t *pud, int flags)
{
	if (flags & (FOLL_GET | FOLL_PIN))
		return NULL;

	return pte_page(*(pte_t *)pud) + ((address & ~PUD_MASK) >> PAGE_SHIFT);
}