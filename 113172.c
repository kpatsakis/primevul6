static inline bool pgtable_page_ctor(struct page *page)
{
	if (!ptlock_init(page))
		return false;
	__SetPageTable(page);
	inc_zone_page_state(page, NR_PAGETABLE);
	return true;
}