void hugetlb_show_meminfo(void)
{
	struct hstate *h;
	int nid;

	if (!hugepages_supported())
		return;

	for_each_node_state(nid, N_MEMORY)
		for_each_hstate(h)
			pr_info("Node %d hugepages_total=%u hugepages_free=%u hugepages_surp=%u hugepages_size=%lukB\n",
				nid,
				h->nr_huge_pages_node[nid],
				h->free_huge_pages_node[nid],
				h->surplus_huge_pages_node[nid],
				huge_page_size(h) / SZ_1K);
}