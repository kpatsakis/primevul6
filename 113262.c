static inline int total_mapcount(struct page *page)
{
	return page_mapcount(page);
}