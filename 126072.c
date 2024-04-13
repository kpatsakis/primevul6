static void destroy_compound_gigantic_page(struct page *page,
					unsigned int order)
{
	__destroy_compound_gigantic_page(page, order, false);
}