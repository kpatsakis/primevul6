static int __init hugepagesz_setup(char *s)
{
	unsigned long size;
	struct hstate *h;

	parsed_valid_hugepagesz = false;
	size = (unsigned long)memparse(s, NULL);

	if (!arch_hugetlb_valid_size(size)) {
		pr_err("HugeTLB: unsupported hugepagesz=%s\n", s);
		return 0;
	}

	h = size_to_hstate(size);
	if (h) {
		/*
		 * hstate for this size already exists.  This is normally
		 * an error, but is allowed if the existing hstate is the
		 * default hstate.  More specifically, it is only allowed if
		 * the number of huge pages for the default hstate was not
		 * previously specified.
		 */
		if (!parsed_default_hugepagesz ||  h != &default_hstate ||
		    default_hstate.max_huge_pages) {
			pr_warn("HugeTLB: hugepagesz=%s specified twice, ignoring\n", s);
			return 0;
		}

		/*
		 * No need to call hugetlb_add_hstate() as hstate already
		 * exists.  But, do set parsed_hstate so that a following
		 * hugepages= parameter will be applied to this hstate.
		 */
		parsed_hstate = h;
		parsed_valid_hugepagesz = true;
		return 1;
	}

	hugetlb_add_hstate(ilog2(size) - PAGE_SHIFT);
	parsed_valid_hugepagesz = true;
	return 1;
}