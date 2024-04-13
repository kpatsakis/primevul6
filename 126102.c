int isolate_or_dissolve_huge_page(struct page *page, struct list_head *list)
{
	struct hstate *h;
	struct page *head;
	int ret = -EBUSY;

	/*
	 * The page might have been dissolved from under our feet, so make sure
	 * to carefully check the state under the lock.
	 * Return success when racing as if we dissolved the page ourselves.
	 */
	spin_lock_irq(&hugetlb_lock);
	if (PageHuge(page)) {
		head = compound_head(page);
		h = page_hstate(head);
	} else {
		spin_unlock_irq(&hugetlb_lock);
		return 0;
	}
	spin_unlock_irq(&hugetlb_lock);

	/*
	 * Fence off gigantic pages as there is a cyclic dependency between
	 * alloc_contig_range and them. Return -ENOMEM as this has the effect
	 * of bailing out right away without further retrying.
	 */
	if (hstate_is_gigantic(h))
		return -ENOMEM;

	if (page_count(head) && isolate_huge_page(head, list))
		ret = 0;
	else if (!page_count(head))
		ret = alloc_and_dissolve_huge_page(h, head, list);

	return ret;
}