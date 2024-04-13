int __alloc_bootmem_huge_page(struct hstate *h, int nid)
{
	struct huge_bootmem_page *m = NULL; /* initialize for clang */
	int nr_nodes, node;

	if (nid >= nr_online_nodes)
		return 0;
	/* do node specific alloc */
	if (nid != NUMA_NO_NODE) {
		m = memblock_alloc_try_nid_raw(huge_page_size(h), huge_page_size(h),
				0, MEMBLOCK_ALLOC_ACCESSIBLE, nid);
		if (!m)
			return 0;
		goto found;
	}
	/* allocate from next node when distributing huge pages */
	for_each_node_mask_to_alloc(h, nr_nodes, node, &node_states[N_MEMORY]) {
		m = memblock_alloc_try_nid_raw(
				huge_page_size(h), huge_page_size(h),
				0, MEMBLOCK_ALLOC_ACCESSIBLE, node);
		/*
		 * Use the beginning of the huge page to store the
		 * huge_bootmem_page struct (until gather_bootmem
		 * puts them into the mem_map).
		 */
		if (!m)
			return 0;
		goto found;
	}

found:
	/* Put them into a private list first because mem_map is not up yet */
	INIT_LIST_HEAD(&m->list);
	list_add(&m->list, &huge_boot_pages);
	m->hstate = h;
	return 1;
}