static int __init hugepages_setup(char *s)
{
	unsigned long *mhp;
	static unsigned long *last_mhp;
	int node = NUMA_NO_NODE;
	int count;
	unsigned long tmp;
	char *p = s;

	if (!parsed_valid_hugepagesz) {
		pr_warn("HugeTLB: hugepages=%s does not follow a valid hugepagesz, ignoring\n", s);
		parsed_valid_hugepagesz = true;
		return 0;
	}

	/*
	 * !hugetlb_max_hstate means we haven't parsed a hugepagesz= parameter
	 * yet, so this hugepages= parameter goes to the "default hstate".
	 * Otherwise, it goes with the previously parsed hugepagesz or
	 * default_hugepagesz.
	 */
	else if (!hugetlb_max_hstate)
		mhp = &default_hstate_max_huge_pages;
	else
		mhp = &parsed_hstate->max_huge_pages;

	if (mhp == last_mhp) {
		pr_warn("HugeTLB: hugepages= specified twice without interleaving hugepagesz=, ignoring hugepages=%s\n", s);
		return 0;
	}

	while (*p) {
		count = 0;
		if (sscanf(p, "%lu%n", &tmp, &count) != 1)
			goto invalid;
		/* Parameter is node format */
		if (p[count] == ':') {
			if (!hugetlb_node_alloc_supported()) {
				pr_warn("HugeTLB: architecture can't support node specific alloc, ignoring!\n");
				return 0;
			}
			node = tmp;
			p += count + 1;
			if (node < 0 || node >= nr_online_nodes)
				goto invalid;
			/* Parse hugepages */
			if (sscanf(p, "%lu%n", &tmp, &count) != 1)
				goto invalid;
			if (!hugetlb_max_hstate)
				default_hugepages_in_node[node] = tmp;
			else
				parsed_hstate->max_huge_pages_node[node] = tmp;
			*mhp += tmp;
			/* Go to parse next node*/
			if (p[count] == ',')
				p += count + 1;
			else
				break;
		} else {
			if (p != s)
				goto invalid;
			*mhp = tmp;
			break;
		}
	}

	/*
	 * Global state is always initialized later in hugetlb_init.
	 * But we need to allocate gigantic hstates here early to still
	 * use the bootmem allocator.
	 */
	if (hugetlb_max_hstate && hstate_is_gigantic(parsed_hstate))
		hugetlb_hstate_alloc_pages(parsed_hstate);

	last_mhp = mhp;

	return 1;

invalid:
	pr_warn("HugeTLB: Invalid hugepages parameter %s\n", p);
	return 0;
}