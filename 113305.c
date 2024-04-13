static inline int page_zone_id(struct page *page)
{
	return (page->flags >> ZONEID_PGSHIFT) & ZONEID_MASK;
}