static inline p4d_t *p4d_alloc(struct mm_struct *mm, pgd_t *pgd,
		unsigned long address)
{
	return (unlikely(pgd_none(*pgd)) && __p4d_alloc(mm, pgd, address)) ?
		NULL : p4d_offset(pgd, address);
}