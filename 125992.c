static long add_reservation_in_range(struct resv_map *resv, long f, long t,
				     struct hugetlb_cgroup *h_cg,
				     struct hstate *h, long *regions_needed)
{
	long add = 0;
	struct list_head *head = &resv->regions;
	long last_accounted_offset = f;
	struct file_region *rg = NULL, *trg = NULL;

	if (regions_needed)
		*regions_needed = 0;

	/* In this loop, we essentially handle an entry for the range
	 * [last_accounted_offset, rg->from), at every iteration, with some
	 * bounds checking.
	 */
	list_for_each_entry_safe(rg, trg, head, link) {
		/* Skip irrelevant regions that start before our range. */
		if (rg->from < f) {
			/* If this region ends after the last accounted offset,
			 * then we need to update last_accounted_offset.
			 */
			if (rg->to > last_accounted_offset)
				last_accounted_offset = rg->to;
			continue;
		}

		/* When we find a region that starts beyond our range, we've
		 * finished.
		 */
		if (rg->from >= t)
			break;

		/* Add an entry for last_accounted_offset -> rg->from, and
		 * update last_accounted_offset.
		 */
		if (rg->from > last_accounted_offset)
			add += hugetlb_resv_map_add(resv, rg,
						    last_accounted_offset,
						    rg->from, h, h_cg,
						    regions_needed);

		last_accounted_offset = rg->to;
	}

	/* Handle the case where our range extends beyond
	 * last_accounted_offset.
	 */
	if (last_accounted_offset < t)
		add += hugetlb_resv_map_add(resv, rg, last_accounted_offset,
					    t, h, h_cg, regions_needed);

	return add;
}