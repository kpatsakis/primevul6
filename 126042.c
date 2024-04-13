int dissolve_free_huge_page(struct page *page)
{
	int rc = -EBUSY;

retry:
	/* Not to disrupt normal path by vainly holding hugetlb_lock */
	if (!PageHuge(page))
		return 0;

	spin_lock_irq(&hugetlb_lock);
	if (!PageHuge(page)) {
		rc = 0;
		goto out;
	}

	if (!page_count(page)) {
		struct page *head = compound_head(page);
		struct hstate *h = page_hstate(head);
		if (h->free_huge_pages - h->resv_huge_pages == 0)
			goto out;

		/*
		 * We should make sure that the page is already on the free list
		 * when it is dissolved.
		 */
		if (unlikely(!HPageFreed(head))) {
			spin_unlock_irq(&hugetlb_lock);
			cond_resched();

			/*
			 * Theoretically, we should return -EBUSY when we
			 * encounter this race. In fact, we have a chance
			 * to successfully dissolve the page if we do a
			 * retry. Because the race window is quite small.
			 * If we seize this opportunity, it is an optimization
			 * for increasing the success rate of dissolving page.
			 */
			goto retry;
		}

		remove_hugetlb_page(h, head, false);
		h->max_huge_pages--;
		spin_unlock_irq(&hugetlb_lock);

		/*
		 * Normally update_and_free_page will allocate required vmemmmap
		 * before freeing the page.  update_and_free_page will fail to
		 * free the page if it can not allocate required vmemmap.  We
		 * need to adjust max_huge_pages if the page is not freed.
		 * Attempt to allocate vmemmmap here so that we can take
		 * appropriate action on failure.
		 */
		rc = alloc_huge_page_vmemmap(h, head);
		if (!rc) {
			/*
			 * Move PageHWPoison flag from head page to the raw
			 * error page, which makes any subpages rather than
			 * the error page reusable.
			 */
			if (PageHWPoison(head) && page != head) {
				SetPageHWPoison(page);
				ClearPageHWPoison(head);
			}
			update_and_free_page(h, head, false);
		} else {
			spin_lock_irq(&hugetlb_lock);
			add_hugetlb_page(h, head, false);
			h->max_huge_pages++;
			spin_unlock_irq(&hugetlb_lock);
		}

		return rc;
	}
out:
	spin_unlock_irq(&hugetlb_lock);
	return rc;
}