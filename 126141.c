static int alloc_and_dissolve_huge_page(struct hstate *h, struct page *old_page,
					struct list_head *list)
{
	gfp_t gfp_mask = htlb_alloc_mask(h) | __GFP_THISNODE;
	int nid = page_to_nid(old_page);
	bool alloc_retry = false;
	struct page *new_page;
	int ret = 0;

	/*
	 * Before dissolving the page, we need to allocate a new one for the
	 * pool to remain stable.  Here, we allocate the page and 'prep' it
	 * by doing everything but actually updating counters and adding to
	 * the pool.  This simplifies and let us do most of the processing
	 * under the lock.
	 */
alloc_retry:
	new_page = alloc_buddy_huge_page(h, gfp_mask, nid, NULL, NULL);
	if (!new_page)
		return -ENOMEM;
	/*
	 * If all goes well, this page will be directly added to the free
	 * list in the pool.  For this the ref count needs to be zero.
	 * Attempt to drop now, and retry once if needed.  It is VERY
	 * unlikely there is another ref on the page.
	 *
	 * If someone else has a reference to the page, it will be freed
	 * when they drop their ref.  Abuse temporary page flag to accomplish
	 * this.  Retry once if there is an inflated ref count.
	 */
	SetHPageTemporary(new_page);
	if (!put_page_testzero(new_page)) {
		if (alloc_retry)
			return -EBUSY;

		alloc_retry = true;
		goto alloc_retry;
	}
	ClearHPageTemporary(new_page);

	__prep_new_huge_page(h, new_page);

retry:
	spin_lock_irq(&hugetlb_lock);
	if (!PageHuge(old_page)) {
		/*
		 * Freed from under us. Drop new_page too.
		 */
		goto free_new;
	} else if (page_count(old_page)) {
		/*
		 * Someone has grabbed the page, try to isolate it here.
		 * Fail with -EBUSY if not possible.
		 */
		spin_unlock_irq(&hugetlb_lock);
		if (!isolate_huge_page(old_page, list))
			ret = -EBUSY;
		spin_lock_irq(&hugetlb_lock);
		goto free_new;
	} else if (!HPageFreed(old_page)) {
		/*
		 * Page's refcount is 0 but it has not been enqueued in the
		 * freelist yet. Race window is small, so we can succeed here if
		 * we retry.
		 */
		spin_unlock_irq(&hugetlb_lock);
		cond_resched();
		goto retry;
	} else {
		/*
		 * Ok, old_page is still a genuine free hugepage. Remove it from
		 * the freelist and decrease the counters. These will be
		 * incremented again when calling __prep_account_new_huge_page()
		 * and enqueue_huge_page() for new_page. The counters will remain
		 * stable since this happens under the lock.
		 */
		remove_hugetlb_page(h, old_page, false);

		/*
		 * Ref count on new page is already zero as it was dropped
		 * earlier.  It can be directly added to the pool free list.
		 */
		__prep_account_new_huge_page(h, nid);
		enqueue_huge_page(h, new_page);

		/*
		 * Pages have been replaced, we can safely free the old one.
		 */
		spin_unlock_irq(&hugetlb_lock);
		update_and_free_page(h, old_page, false);
	}

	return ret;

free_new:
	spin_unlock_irq(&hugetlb_lock);
	/* Page has a zero ref count, but needs a ref to be freed */
	set_page_refcounted(new_page);
	update_and_free_page(h, new_page, false);

	return ret;
}