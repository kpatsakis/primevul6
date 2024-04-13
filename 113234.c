static inline spinlock_t *pmd_lockptr(struct mm_struct *mm, pmd_t *pmd)
{
	return ptlock_ptr(pmd_to_page(pmd));
}