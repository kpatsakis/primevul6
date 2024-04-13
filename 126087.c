static struct page *remove_pool_huge_page(struct hstate *h,
						nodemask_t *nodes_allowed,
						 bool acct_surplus)
{
	int nr_nodes, node;
	struct page *page = NULL;

	lockdep_assert_held(&hugetlb_lock);
	for_each_node_mask_to_free(h, nr_nodes, node, nodes_allowed) {
		/*
		 * If we're returning unused surplus pages, only examine
		 * nodes with surplus pages.
		 */
		if ((!acct_surplus || h->surplus_huge_pages_node[node]) &&
		    !list_empty(&h->hugepage_freelists[node])) {
			page = list_entry(h->hugepage_freelists[node].next,
					  struct page, lru);
			remove_hugetlb_page(h, page, acct_surplus);
			break;
		}
	}

	return page;
}