static inline spinlock_t *ptlock_ptr(struct page *page)
{
	return page->ptl;
}