void __init hugetlb_cma_reserve(int order)
{
	unsigned long size, reserved, per_node;
	bool node_specific_cma_alloc = false;
	int nid;

	cma_reserve_called = true;

	if (!hugetlb_cma_size)
		return;

	for (nid = 0; nid < MAX_NUMNODES; nid++) {
		if (hugetlb_cma_size_in_node[nid] == 0)
			continue;

		if (!node_state(nid, N_ONLINE)) {
			pr_warn("hugetlb_cma: invalid node %d specified\n", nid);
			hugetlb_cma_size -= hugetlb_cma_size_in_node[nid];
			hugetlb_cma_size_in_node[nid] = 0;
			continue;
		}

		if (hugetlb_cma_size_in_node[nid] < (PAGE_SIZE << order)) {
			pr_warn("hugetlb_cma: cma area of node %d should be at least %lu MiB\n",
				nid, (PAGE_SIZE << order) / SZ_1M);
			hugetlb_cma_size -= hugetlb_cma_size_in_node[nid];
			hugetlb_cma_size_in_node[nid] = 0;
		} else {
			node_specific_cma_alloc = true;
		}
	}

	/* Validate the CMA size again in case some invalid nodes specified. */
	if (!hugetlb_cma_size)
		return;

	if (hugetlb_cma_size < (PAGE_SIZE << order)) {
		pr_warn("hugetlb_cma: cma area should be at least %lu MiB\n",
			(PAGE_SIZE << order) / SZ_1M);
		hugetlb_cma_size = 0;
		return;
	}

	if (!node_specific_cma_alloc) {
		/*
		 * If 3 GB area is requested on a machine with 4 numa nodes,
		 * let's allocate 1 GB on first three nodes and ignore the last one.
		 */
		per_node = DIV_ROUND_UP(hugetlb_cma_size, nr_online_nodes);
		pr_info("hugetlb_cma: reserve %lu MiB, up to %lu MiB per node\n",
			hugetlb_cma_size / SZ_1M, per_node / SZ_1M);
	}

	reserved = 0;
	for_each_node_state(nid, N_ONLINE) {
		int res;
		char name[CMA_MAX_NAME];

		if (node_specific_cma_alloc) {
			if (hugetlb_cma_size_in_node[nid] == 0)
				continue;

			size = hugetlb_cma_size_in_node[nid];
		} else {
			size = min(per_node, hugetlb_cma_size - reserved);
		}

		size = round_up(size, PAGE_SIZE << order);

		snprintf(name, sizeof(name), "hugetlb%d", nid);
		/*
		 * Note that 'order per bit' is based on smallest size that
		 * may be returned to CMA allocator in the case of
		 * huge page demotion.
		 */
		res = cma_declare_contiguous_nid(0, size, 0,
						PAGE_SIZE << HUGETLB_PAGE_ORDER,
						 0, false, name,
						 &hugetlb_cma[nid], nid);
		if (res) {
			pr_warn("hugetlb_cma: reservation failed: err %d, node %d",
				res, nid);
			continue;
		}

		reserved += size;
		pr_info("hugetlb_cma: reserved %lu MiB on node %d\n",
			size / SZ_1M, nid);

		if (reserved >= hugetlb_cma_size)
			break;
	}

	if (!reserved)
		/*
		 * hugetlb_cma_size is used to determine if allocations from
		 * cma are possible.  Set to zero if no cma regions are set up.
		 */
		hugetlb_cma_size = 0;
}