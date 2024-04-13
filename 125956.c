static void __update_and_free_page(struct hstate *h, struct page *page)
{
	int i;
	struct page *subpage = page;

	if (hstate_is_gigantic(h) && !gigantic_page_runtime_supported())
		return;

	if (alloc_huge_page_vmemmap(h, page)) {
		spin_lock_irq(&hugetlb_lock);
		/*
		 * If we cannot allocate vmemmap pages, just refuse to free the
		 * page and put the page back on the hugetlb free list and treat
		 * as a surplus page.
		 */
		add_hugetlb_page(h, page, true);
		spin_unlock_irq(&hugetlb_lock);
		return;
	}

	for (i = 0; i < pages_per_huge_page(h);
	     i++, subpage = mem_map_next(subpage, page, i)) {
		subpage->flags &= ~(1 << PG_locked | 1 << PG_error |
				1 << PG_referenced | 1 << PG_dirty |
				1 << PG_active | 1 << PG_private |
				1 << PG_writeback);
	}

	/*
	 * Non-gigantic pages demoted from CMA allocated gigantic pages
	 * need to be given back to CMA in free_gigantic_page.
	 */
	if (hstate_is_gigantic(h) ||
	    hugetlb_cma_page(page, huge_page_order(h))) {
		destroy_compound_gigantic_page(page, huge_page_order(h));
		free_gigantic_page(page, huge_page_order(h));
	} else {
		__free_pages(page, huge_page_order(h));
	}
}