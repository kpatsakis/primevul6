static inline enum zone_type page_zonenum(const struct page *page)
{
	return (page->flags >> ZONES_PGSHIFT) & ZONES_MASK;
}