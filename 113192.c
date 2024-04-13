static inline void page_cpupid_reset_last(struct page *page)
{
	page->_last_cpupid = -1 & LAST_CPUPID_MASK;
}