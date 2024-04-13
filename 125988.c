static void __init hugetlb_hstate_alloc_pages(struct hstate *h)
{
	unsigned long i;
	nodemask_t *node_alloc_noretry;
	bool node_specific_alloc = false;

	/* skip gigantic hugepages allocation if hugetlb_cma enabled */
	if (hstate_is_gigantic(h) && hugetlb_cma_size) {
		pr_warn_once("HugeTLB: hugetlb_cma is enabled, skip boot time allocation\n");
		return;
	}

	/* do node specific alloc */
	for (i = 0; i < nr_online_nodes; i++) {
		if (h->max_huge_pages_node[i] > 0) {
			hugetlb_hstate_alloc_pages_onenode(h, i);
			node_specific_alloc = true;
		}
	}

	if (node_specific_alloc)
		return;

	/* below will do all node balanced alloc */
	if (!hstate_is_gigantic(h)) {
		/*
		 * Bit mask controlling how hard we retry per-node allocations.
		 * Ignore errors as lower level routines can deal with
		 * node_alloc_noretry == NULL.  If this kmalloc fails at boot
		 * time, we are likely in bigger trouble.
		 */
		node_alloc_noretry = kmalloc(sizeof(*node_alloc_noretry),
						GFP_KERNEL);
	} else {
		/* allocations done at boot time */
		node_alloc_noretry = NULL;
	}

	/* bit mask controlling how hard we retry per-node allocations */
	if (node_alloc_noretry)
		nodes_clear(*node_alloc_noretry);

	for (i = 0; i < h->max_huge_pages; ++i) {
		if (hstate_is_gigantic(h)) {
			if (!alloc_bootmem_huge_page(h, NUMA_NO_NODE))
				break;
		} else if (!alloc_pool_huge_page(h,
					 &node_states[N_MEMORY],
					 node_alloc_noretry))
			break;
		cond_resched();
	}
	if (i < h->max_huge_pages) {
		char buf[32];

		string_get_size(huge_page_size(h), 1, STRING_UNITS_2, buf, 32);
		pr_warn("HugeTLB: allocating %lu of page size %s failed.  Only allocated %lu hugepages.\n",
			h->max_huge_pages, buf, i);
		h->max_huge_pages = i;
	}
	kfree(node_alloc_noretry);
}