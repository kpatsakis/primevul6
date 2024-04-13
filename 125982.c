static void remove_hugetlb_page(struct hstate *h, struct page *page,
							bool adjust_surplus)
{
	__remove_hugetlb_page(h, page, adjust_surplus, false);
}