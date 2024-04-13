static inline void set_page_pfmemalloc(struct page *page)
{
	page->index = -1UL;
}