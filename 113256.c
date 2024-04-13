static inline int get_page_unless_zero(struct page *page)
{
	return page_ref_add_unless(page, 1, 0);
}