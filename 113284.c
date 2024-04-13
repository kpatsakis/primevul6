static inline int page_mapcount(struct page *page)
{
	VM_BUG_ON_PAGE(PageSlab(page), page);

	if (unlikely(PageCompound(page)))
		return __page_mapcount(page);
	return atomic_read(&page->_mapcount) + 1;
}