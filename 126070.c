static bool hugetlb_cma_page(struct page *page, unsigned int order)
{
	return cma_pages_valid(hugetlb_cma[page_to_nid(page)], page,
				1 << order);
}