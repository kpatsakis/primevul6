static inline spinlock_t *pud_lockptr(struct mm_struct *mm, pud_t *pud)
{
	return &mm->page_table_lock;
}