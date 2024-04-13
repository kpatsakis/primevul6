static inline void free_reserved_page(struct page *page)
{
	__free_reserved_page(page);
	adjust_managed_page_count(page, 1);
}