static void __init hugetlb_hstate_alloc_pages_onenode(struct hstate *h, int nid)
{
	unsigned long i;
	char buf[32];

	for (i = 0; i < h->max_huge_pages_node[nid]; ++i) {
		if (hstate_is_gigantic(h)) {
			if (!alloc_bootmem_huge_page(h, nid))
				break;
		} else {
			struct page *page;
			gfp_t gfp_mask = htlb_alloc_mask(h) | __GFP_THISNODE;

			page = alloc_fresh_huge_page(h, gfp_mask, nid,
					&node_states[N_MEMORY], NULL);
			if (!page)
				break;
			put_page(page); /* free it into the hugepage allocator */
		}
		cond_resched();
	}
	if (i == h->max_huge_pages_node[nid])
		return;

	string_get_size(huge_page_size(h), 1, STRING_UNITS_2, buf, 32);
	pr_warn("HugeTLB: allocating %u of page size %s failed node%d.  Only allocated %lu hugepages.\n",
		h->max_huge_pages_node[nid], buf, nid, i);
	h->max_huge_pages -= (h->max_huge_pages_node[nid] - i);
	h->max_huge_pages_node[nid] = i;
}