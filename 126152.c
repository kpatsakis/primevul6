static int __init hugetlb_init(void)
{
	int i;

	BUILD_BUG_ON(sizeof_field(struct page, private) * BITS_PER_BYTE <
			__NR_HPAGEFLAGS);

	if (!hugepages_supported()) {
		if (hugetlb_max_hstate || default_hstate_max_huge_pages)
			pr_warn("HugeTLB: huge pages not supported, ignoring associated command-line parameters\n");
		return 0;
	}

	/*
	 * Make sure HPAGE_SIZE (HUGETLB_PAGE_ORDER) hstate exists.  Some
	 * architectures depend on setup being done here.
	 */
	hugetlb_add_hstate(HUGETLB_PAGE_ORDER);
	if (!parsed_default_hugepagesz) {
		/*
		 * If we did not parse a default huge page size, set
		 * default_hstate_idx to HPAGE_SIZE hstate. And, if the
		 * number of huge pages for this default size was implicitly
		 * specified, set that here as well.
		 * Note that the implicit setting will overwrite an explicit
		 * setting.  A warning will be printed in this case.
		 */
		default_hstate_idx = hstate_index(size_to_hstate(HPAGE_SIZE));
		if (default_hstate_max_huge_pages) {
			if (default_hstate.max_huge_pages) {
				char buf[32];

				string_get_size(huge_page_size(&default_hstate),
					1, STRING_UNITS_2, buf, 32);
				pr_warn("HugeTLB: Ignoring hugepages=%lu associated with %s page size\n",
					default_hstate.max_huge_pages, buf);
				pr_warn("HugeTLB: Using hugepages=%lu for number of default huge pages\n",
					default_hstate_max_huge_pages);
			}
			default_hstate.max_huge_pages =
				default_hstate_max_huge_pages;

			for (i = 0; i < nr_online_nodes; i++)
				default_hstate.max_huge_pages_node[i] =
					default_hugepages_in_node[i];
		}
	}

	hugetlb_cma_check();
	hugetlb_init_hstates();
	gather_bootmem_prealloc();
	report_hugepages();

	hugetlb_sysfs_init();
	hugetlb_register_all_nodes();
	hugetlb_cgroup_file_init();

#ifdef CONFIG_SMP
	num_fault_mutexes = roundup_pow_of_two(8 * num_possible_cpus());
#else
	num_fault_mutexes = 1;
#endif
	hugetlb_fault_mutex_table =
		kmalloc_array(num_fault_mutexes, sizeof(struct mutex),
			      GFP_KERNEL);
	BUG_ON(!hugetlb_fault_mutex_table);

	for (i = 0; i < num_fault_mutexes; i++)
		mutex_init(&hugetlb_fault_mutex_table[i]);
	return 0;
}