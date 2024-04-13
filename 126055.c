static void __init hugetlb_init_hstates(void)
{
	struct hstate *h, *h2;

	for_each_hstate(h) {
		if (minimum_order > huge_page_order(h))
			minimum_order = huge_page_order(h);

		/* oversize hugepages were init'ed in early boot */
		if (!hstate_is_gigantic(h))
			hugetlb_hstate_alloc_pages(h);

		/*
		 * Set demote order for each hstate.  Note that
		 * h->demote_order is initially 0.
		 * - We can not demote gigantic pages if runtime freeing
		 *   is not supported, so skip this.
		 * - If CMA allocation is possible, we can not demote
		 *   HUGETLB_PAGE_ORDER or smaller size pages.
		 */
		if (hstate_is_gigantic(h) && !gigantic_page_runtime_supported())
			continue;
		if (hugetlb_cma_size && h->order <= HUGETLB_PAGE_ORDER)
			continue;
		for_each_hstate(h2) {
			if (h2 == h)
				continue;
			if (h2->order < h->order &&
			    h2->order > h->demote_order)
				h->demote_order = h2->order;
		}
	}
	VM_BUG_ON(minimum_order == UINT_MAX);
}