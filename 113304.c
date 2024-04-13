static inline void pgtable_pmd_page_dtor(struct page *page)
{
#ifdef CONFIG_TRANSPARENT_HUGEPAGE
	VM_BUG_ON_PAGE(page->pmd_huge_pte, page);
#endif
	ptlock_free(page);
}