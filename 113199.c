static inline int page_cpupid_last(struct page *page)
{
	return page_to_nid(page); /* XXX */
}