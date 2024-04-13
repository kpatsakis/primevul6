static inline void page_cpupid_reset_last(struct page *page)
{
	page->flags |= LAST_CPUPID_MASK << LAST_CPUPID_PGSHIFT;
}