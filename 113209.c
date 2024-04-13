static inline struct page *virt_to_head_page(const void *x)
{
	struct page *page = virt_to_page(x);

	return compound_head(page);
}