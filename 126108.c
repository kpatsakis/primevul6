static bool prep_compound_gigantic_page_for_demote(struct page *page,
							unsigned int order)
{
	return __prep_compound_gigantic_page(page, order, true);
}