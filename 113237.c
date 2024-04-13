static inline bool is_device_public_page(const struct page *page)
{
	return is_zone_device_page(page) &&
		page->pgmap->type == MEMORY_DEVICE_PUBLIC;
}