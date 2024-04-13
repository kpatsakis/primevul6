static inline bool page_is_pfmemalloc(struct page *page)
{
	/*
	 * Page index cannot be this large so this must be
	 * a pfmemalloc page.
	 */
	return page->index == -1UL;
}