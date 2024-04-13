static inline int page_cpupid_xchg_last(struct page *page, int cpupid)
{
	return page_to_nid(page); /* XXX */
}