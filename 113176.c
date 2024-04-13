static inline compound_page_dtor *get_compound_page_dtor(struct page *page)
{
	VM_BUG_ON_PAGE(page[1].compound_dtor >= NR_COMPOUND_DTORS, page);
	return compound_page_dtors[page[1].compound_dtor];
}