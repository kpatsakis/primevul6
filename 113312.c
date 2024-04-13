static inline void set_page_address(struct page *page, void *address)
{
	page->virtual = address;
}