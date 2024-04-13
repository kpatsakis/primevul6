static void try_to_free_low(struct hstate *h, unsigned long count,
						nodemask_t *nodes_allowed)
{
	int i;
	LIST_HEAD(page_list);

	lockdep_assert_held(&hugetlb_lock);
	if (hstate_is_gigantic(h))
		return;

	/*
	 * Collect pages to be freed on a list, and free after dropping lock
	 */
	for_each_node_mask(i, *nodes_allowed) {
		struct page *page, *next;
		struct list_head *freel = &h->hugepage_freelists[i];
		list_for_each_entry_safe(page, next, freel, lru) {
			if (count >= h->nr_huge_pages)
				goto out;
			if (PageHighMem(page))
				continue;
			remove_hugetlb_page(h, page, false);
			list_add(&page->lru, &page_list);
		}
	}

out:
	spin_unlock_irq(&hugetlb_lock);
	update_and_free_pages_bulk(h, &page_list);
	spin_lock_irq(&hugetlb_lock);
}