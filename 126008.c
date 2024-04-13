static int __init default_hugepagesz_setup(char *s)
{
	unsigned long size;
	int i;

	parsed_valid_hugepagesz = false;
	if (parsed_default_hugepagesz) {
		pr_err("HugeTLB: default_hugepagesz previously specified, ignoring %s\n", s);
		return 0;
	}

	size = (unsigned long)memparse(s, NULL);

	if (!arch_hugetlb_valid_size(size)) {
		pr_err("HugeTLB: unsupported default_hugepagesz=%s\n", s);
		return 0;
	}

	hugetlb_add_hstate(ilog2(size) - PAGE_SHIFT);
	parsed_valid_hugepagesz = true;
	parsed_default_hugepagesz = true;
	default_hstate_idx = hstate_index(size_to_hstate(size));

	/*
	 * The number of default huge pages (for this size) could have been
	 * specified as the first hugetlb parameter: hugepages=X.  If so,
	 * then default_hstate_max_huge_pages is set.  If the default huge
	 * page size is gigantic (>= MAX_ORDER), then the pages must be
	 * allocated here from bootmem allocator.
	 */
	if (default_hstate_max_huge_pages) {
		default_hstate.max_huge_pages = default_hstate_max_huge_pages;
		for (i = 0; i < nr_online_nodes; i++)
			default_hstate.max_huge_pages_node[i] =
				default_hugepages_in_node[i];
		if (hstate_is_gigantic(&default_hstate))
			hugetlb_hstate_alloc_pages(&default_hstate);
		default_hstate_max_huge_pages = 0;
	}

	return 1;
}