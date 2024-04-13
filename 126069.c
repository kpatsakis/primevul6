static long region_chg(struct resv_map *resv, long f, long t,
		       long *out_regions_needed)
{
	long chg = 0;

	spin_lock(&resv->lock);

	/* Count how many hugepages in this range are NOT represented. */
	chg = add_reservation_in_range(resv, f, t, NULL, NULL,
				       out_regions_needed);

	if (*out_regions_needed == 0)
		*out_regions_needed = 1;

	if (allocate_file_region_entries(resv, *out_regions_needed))
		return -ENOMEM;

	resv->adds_in_progress += *out_regions_needed;

	spin_unlock(&resv->lock);
	return chg;
}