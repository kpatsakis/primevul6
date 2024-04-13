static inline void *page_address(const struct page *page)
{
	return page->virtual;
}