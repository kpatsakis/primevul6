static inline bool pgtable_pmd_page_ctor(struct page *page)
{
#ifdef CONFIG_TRANSPARENT_HUGEPAGE
	page->pmd_huge_pte = NULL;
#endif
	return ptlock_init(page);
}