static inline void pgtable_page_dtor(struct page *page)
{
	ptlock_free(page);
	__ClearPageTable(page);
	dec_zone_page_state(page, NR_PAGETABLE);
}