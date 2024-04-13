void hugetlb_report_meminfo(struct seq_file *m)
{
	struct hstate *h;
	unsigned long total = 0;

	if (!hugepages_supported())
		return;

	for_each_hstate(h) {
		unsigned long count = h->nr_huge_pages;

		total += huge_page_size(h) * count;

		if (h == &default_hstate)
			seq_printf(m,
				   "HugePages_Total:   %5lu\n"
				   "HugePages_Free:    %5lu\n"
				   "HugePages_Rsvd:    %5lu\n"
				   "HugePages_Surp:    %5lu\n"
				   "Hugepagesize:   %8lu kB\n",
				   count,
				   h->free_huge_pages,
				   h->resv_huge_pages,
				   h->surplus_huge_pages,
				   huge_page_size(h) / SZ_1K);
	}

	seq_printf(m, "Hugetlb:        %8lu kB\n", total / SZ_1K);
}