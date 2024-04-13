static inline void clear_page_pfmemalloc(struct page *page)
{
	page->index = 0;
}