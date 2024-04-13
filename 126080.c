static ssize_t __nr_hugepages_store_common(bool obey_mempolicy,
					   struct hstate *h, int nid,
					   unsigned long count, size_t len)
{
	int err;
	nodemask_t nodes_allowed, *n_mask;

	if (hstate_is_gigantic(h) && !gigantic_page_runtime_supported())
		return -EINVAL;

	if (nid == NUMA_NO_NODE) {
		/*
		 * global hstate attribute
		 */
		if (!(obey_mempolicy &&
				init_nodemask_of_mempolicy(&nodes_allowed)))
			n_mask = &node_states[N_MEMORY];
		else
			n_mask = &nodes_allowed;
	} else {
		/*
		 * Node specific request.  count adjustment happens in
		 * set_max_huge_pages() after acquiring hugetlb_lock.
		 */
		init_nodemask_of_node(&nodes_allowed, nid);
		n_mask = &nodes_allowed;
	}

	err = set_max_huge_pages(h, count, nid, n_mask);

	return err ? err : len;
}