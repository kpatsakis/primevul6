static int alloc_pool_huge_page(struct hstate *h, nodemask_t *nodes_allowed,
				nodemask_t *node_alloc_noretry)
{
	struct page *page;
	int nr_nodes, node;
	gfp_t gfp_mask = htlb_alloc_mask(h) | __GFP_THISNODE;

	for_each_node_mask_to_alloc(h, nr_nodes, node, nodes_allowed) {
		page = alloc_fresh_huge_page(h, gfp_mask, node, nodes_allowed,
						node_alloc_noretry);
		if (page)
			break;
	}

	if (!page)
		return 0;

	put_page(page); /* free it into the hugepage allocator */

	return 1;
}