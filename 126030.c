static struct page *alloc_gigantic_page(struct hstate *h, gfp_t gfp_mask,
		int nid, nodemask_t *nodemask)
{
	unsigned long nr_pages = pages_per_huge_page(h);
	if (nid == NUMA_NO_NODE)
		nid = numa_mem_id();

#ifdef CONFIG_CMA
	{
		struct page *page;
		int node;

		if (hugetlb_cma[nid]) {
			page = cma_alloc(hugetlb_cma[nid], nr_pages,
					huge_page_order(h), true);
			if (page)
				return page;
		}

		if (!(gfp_mask & __GFP_THISNODE)) {
			for_each_node_mask(node, *nodemask) {
				if (node == nid || !hugetlb_cma[node])
					continue;

				page = cma_alloc(hugetlb_cma[node], nr_pages,
						huge_page_order(h), true);
				if (page)
					return page;
			}
		}
	}
#endif

	return alloc_contig_pages(nr_pages, gfp_mask, nid, nodemask);
}