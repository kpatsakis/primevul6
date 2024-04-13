static void __remove_hugetlb_page(struct hstate *h, struct page *page,
							bool adjust_surplus,
							bool demote)
{
	int nid = page_to_nid(page);

	VM_BUG_ON_PAGE(hugetlb_cgroup_from_page(page), page);
	VM_BUG_ON_PAGE(hugetlb_cgroup_from_page_rsvd(page), page);

	lockdep_assert_held(&hugetlb_lock);
	if (hstate_is_gigantic(h) && !gigantic_page_runtime_supported())
		return;

	list_del(&page->lru);

	if (HPageFreed(page)) {
		h->free_huge_pages--;
		h->free_huge_pages_node[nid]--;
	}
	if (adjust_surplus) {
		h->surplus_huge_pages--;
		h->surplus_huge_pages_node[nid]--;
	}

	/*
	 * Very subtle
	 *
	 * For non-gigantic pages set the destructor to the normal compound
	 * page dtor.  This is needed in case someone takes an additional
	 * temporary ref to the page, and freeing is delayed until they drop
	 * their reference.
	 *
	 * For gigantic pages set the destructor to the null dtor.  This
	 * destructor will never be called.  Before freeing the gigantic
	 * page destroy_compound_gigantic_page will turn the compound page
	 * into a simple group of pages.  After this the destructor does not
	 * apply.
	 *
	 * This handles the case where more than one ref is held when and
	 * after update_and_free_page is called.
	 *
	 * In the case of demote we do not ref count the page as it will soon
	 * be turned into a page of smaller size.
	 */
	if (!demote)
		set_page_refcounted(page);
	if (hstate_is_gigantic(h))
		set_compound_page_dtor(page, NULL_COMPOUND_DTOR);
	else
		set_compound_page_dtor(page, COMPOUND_PAGE_DTOR);

	h->nr_huge_pages--;
	h->nr_huge_pages_node[nid]--;
}