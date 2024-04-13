static struct page *dequeue_huge_page_node_exact(struct hstate *h, int nid)
{
	struct page *page;
	bool pin = !!(current->flags & PF_MEMALLOC_PIN);

	lockdep_assert_held(&hugetlb_lock);
	list_for_each_entry(page, &h->hugepage_freelists[nid], lru) {
		if (pin && !is_pinnable_page(page))
			continue;

		if (PageHWPoison(page))
			continue;

		list_move(&page->lru, &h->hugepage_activelist);
		set_page_refcounted(page);
		ClearHPageFreed(page);
		h->free_huge_pages--;
		h->free_huge_pages_node[nid]--;
		return page;
	}

	return NULL;
}