static inline void mark_page_reserved(struct page *page)
{
	SetPageReserved(page);
	adjust_managed_page_count(page, -1);
}