void __init hugetlb_add_hstate(unsigned int order)
{
	struct hstate *h;
	unsigned long i;

	if (size_to_hstate(PAGE_SIZE << order)) {
		return;
	}
	BUG_ON(hugetlb_max_hstate >= HUGE_MAX_HSTATE);
	BUG_ON(order == 0);
	h = &hstates[hugetlb_max_hstate++];
	mutex_init(&h->resize_lock);
	h->order = order;
	h->mask = ~(huge_page_size(h) - 1);
	for (i = 0; i < MAX_NUMNODES; ++i)
		INIT_LIST_HEAD(&h->hugepage_freelists[i]);
	INIT_LIST_HEAD(&h->hugepage_activelist);
	h->next_nid_to_alloc = first_memory_node;
	h->next_nid_to_free = first_memory_node;
	snprintf(h->name, HSTATE_NAME_LEN, "hugepages-%lukB",
					huge_page_size(h)/1024);
	hugetlb_vmemmap_init(h);

	parsed_hstate = h;
}