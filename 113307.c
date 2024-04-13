kernel_map_pages(struct page *page, int numpages, int enable)
{
	if (!debug_pagealloc_enabled())
		return;

	__kernel_map_pages(page, numpages, enable);
}