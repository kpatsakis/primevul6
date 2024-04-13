static void update_and_free_page(struct hstate *h, struct page *page,
				 bool atomic)
{
	if (!HPageVmemmapOptimized(page) || !atomic) {
		__update_and_free_page(h, page);
		return;
	}

	/*
	 * Defer freeing to avoid using GFP_ATOMIC to allocate vmemmap pages.
	 *
	 * Only call schedule_work() if hpage_freelist is previously
	 * empty. Otherwise, schedule_work() had been called but the workfn
	 * hasn't retrieved the list yet.
	 */
	if (llist_add((struct llist_node *)&page->mapping, &hpage_freelist))
		schedule_work(&free_hpage_work);
}