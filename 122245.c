static inline int pud_none_or_clear_bad(pud_t *pud)
{
	if (pud_none(*pud))
		return 1;
	if (unlikely(pud_bad(*pud))) {
		pud_clear_bad(pud);
		return 1;
	}
	return 0;
}