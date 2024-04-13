static void __prep_new_huge_page(struct hstate *h, struct page *page)
{
	free_huge_page_vmemmap(h, page);
	INIT_LIST_HEAD(&page->lru);
	set_compound_page_dtor(page, HUGETLB_PAGE_DTOR);
	hugetlb_set_page_subpool(page, NULL);
	set_hugetlb_cgroup(page, NULL);
	set_hugetlb_cgroup_rsvd(page, NULL);
}