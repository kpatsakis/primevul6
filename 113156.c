static inline void __free_reserved_page(struct page *page)
{
	ClearPageReserved(page);
	init_page_count(page);
	__free_page(page);
}