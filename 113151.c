static __always_inline void *lowmem_page_address(const struct page *page)
{
	return page_to_virt(page);
}