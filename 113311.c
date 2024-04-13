static inline void set_compound_page_dtor(struct page *page,
		enum compound_dtor_id compound_dtor)
{
	VM_BUG_ON_PAGE(compound_dtor >= NR_COMPOUND_DTORS, page);
	page[1].compound_dtor = compound_dtor;
}