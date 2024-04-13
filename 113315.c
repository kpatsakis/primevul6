static inline bool is_zone_device_page(const struct page *page)
{
	return page_zonenum(page) == ZONE_DEVICE;
}