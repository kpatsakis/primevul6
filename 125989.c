static void destroy_compound_hugetlb_page_for_demote(struct page *page,
					unsigned int order)
{
	__destroy_compound_gigantic_page(page, order, true);
}