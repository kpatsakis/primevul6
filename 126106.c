	__must_hold(&hugetlb_lock)
{
	int nr_nodes, node;
	struct page *page;
	int rc = 0;

	lockdep_assert_held(&hugetlb_lock);

	/* We should never get here if no demote order */
	if (!h->demote_order) {
		pr_warn("HugeTLB: NULL demote order passed to demote_pool_huge_page.\n");
		return -EINVAL;		/* internal error */
	}

	for_each_node_mask_to_free(h, nr_nodes, node, nodes_allowed) {
		if (!list_empty(&h->hugepage_freelists[node])) {
			page = list_entry(h->hugepage_freelists[node].next,
					struct page, lru);
			rc = demote_free_huge_page(h, page);
			break;
		}
	}

	return rc;
}