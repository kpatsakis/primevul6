static void return_unused_surplus_pages(struct hstate *h,
					unsigned long unused_resv_pages)
{
	unsigned long nr_pages;
	struct page *page;
	LIST_HEAD(page_list);

	lockdep_assert_held(&hugetlb_lock);
	/* Uncommit the reservation */
	h->resv_huge_pages -= unused_resv_pages;

	/* Cannot return gigantic pages currently */
	if (hstate_is_gigantic(h))
		goto out;

	/*
	 * Part (or even all) of the reservation could have been backed
	 * by pre-allocated pages. Only free surplus pages.
	 */
	nr_pages = min(unused_resv_pages, h->surplus_huge_pages);

	/*
	 * We want to release as many surplus pages as possible, spread
	 * evenly across all nodes with memory. Iterate across these nodes
	 * until we can no longer free unreserved surplus pages. This occurs
	 * when the nodes with surplus pages have no free pages.
	 * remove_pool_huge_page() will balance the freed pages across the
	 * on-line nodes with memory and will handle the hstate accounting.
	 */
	while (nr_pages--) {
		page = remove_pool_huge_page(h, &node_states[N_MEMORY], 1);
		if (!page)
			goto out;

		list_add(&page->lru, &page_list);
	}

out:
	spin_unlock_irq(&hugetlb_lock);
	update_and_free_pages_bulk(h, &page_list);
	spin_lock_irq(&hugetlb_lock);
}