static inline void set_compound_order(struct page *page, unsigned int order)
{
	page[1].compound_order = order;
}