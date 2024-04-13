static inline pgoff_t page_index(struct page *page)
{
	if (unlikely(PageSwapCache(page)))
		return __page_file_index(page);
	return page->index;
}