static void free_hpage_workfn(struct work_struct *work)
{
	struct llist_node *node;

	node = llist_del_all(&hpage_freelist);

	while (node) {
		struct page *page;
		struct hstate *h;

		page = container_of((struct address_space **)node,
				     struct page, mapping);
		node = node->next;
		page->mapping = NULL;
		/*
		 * The VM_BUG_ON_PAGE(!PageHuge(page), page) in page_hstate()
		 * is going to trigger because a previous call to
		 * remove_hugetlb_page() will set_compound_page_dtor(page,
		 * NULL_COMPOUND_DTOR), so do not use page_hstate() directly.
		 */
		h = size_to_hstate(page_size(page));

		__update_and_free_page(h, page);

		cond_resched();
	}
}