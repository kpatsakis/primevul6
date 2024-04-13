static inline int page_cpupid_last(struct page *page)
{
	return page->_last_cpupid;
}