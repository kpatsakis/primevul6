int PageHeadHuge(struct page *page_head)
{
	if (!PageHead(page_head))
		return 0;

	return page_head[1].compound_dtor == HUGETLB_PAGE_DTOR;
}