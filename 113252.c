static inline int page_to_nid(const struct page *page)
{
	struct page *p = (struct page *)page;

	return (PF_POISONED_CHECK(p)->flags >> NODES_PGSHIFT) & NODES_MASK;
}