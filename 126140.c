static long region_add(struct resv_map *resv, long f, long t,
		       long in_regions_needed, struct hstate *h,
		       struct hugetlb_cgroup *h_cg)
{
	long add = 0, actual_regions_needed = 0;

	spin_lock(&resv->lock);
retry:

	/* Count how many regions are actually needed to execute this add. */
	add_reservation_in_range(resv, f, t, NULL, NULL,
				 &actual_regions_needed);

	/*
	 * Check for sufficient descriptors in the cache to accommodate
	 * this add operation. Note that actual_regions_needed may be greater
	 * than in_regions_needed, as the resv_map may have been modified since
	 * the region_chg call. In this case, we need to make sure that we
	 * allocate extra entries, such that we have enough for all the
	 * existing adds_in_progress, plus the excess needed for this
	 * operation.
	 */
	if (actual_regions_needed > in_regions_needed &&
	    resv->region_cache_count <
		    resv->adds_in_progress +
			    (actual_regions_needed - in_regions_needed)) {
		/* region_add operation of range 1 should never need to
		 * allocate file_region entries.
		 */
		VM_BUG_ON(t - f <= 1);

		if (allocate_file_region_entries(
			    resv, actual_regions_needed - in_regions_needed)) {
			return -ENOMEM;
		}

		goto retry;
	}

	add = add_reservation_in_range(resv, f, t, h_cg, h, NULL);

	resv->adds_in_progress -= in_regions_needed;

	spin_unlock(&resv->lock);
	return add;
}