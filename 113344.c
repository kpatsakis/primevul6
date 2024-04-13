static inline int compound_mapcount(struct page *page)
{
	VM_BUG_ON_PAGE(!PageCompound(page), page);
	page = compound_head(page);
	return atomic_read(compound_mapcount_ptr(page)) + 1;
}