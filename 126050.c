	__must_hold(&resv->lock)
{
	struct list_head allocated_regions;
	int to_allocate = 0, i = 0;
	struct file_region *trg = NULL, *rg = NULL;

	VM_BUG_ON(regions_needed < 0);

	INIT_LIST_HEAD(&allocated_regions);

	/*
	 * Check for sufficient descriptors in the cache to accommodate
	 * the number of in progress add operations plus regions_needed.
	 *
	 * This is a while loop because when we drop the lock, some other call
	 * to region_add or region_del may have consumed some region_entries,
	 * so we keep looping here until we finally have enough entries for
	 * (adds_in_progress + regions_needed).
	 */
	while (resv->region_cache_count <
	       (resv->adds_in_progress + regions_needed)) {
		to_allocate = resv->adds_in_progress + regions_needed -
			      resv->region_cache_count;

		/* At this point, we should have enough entries in the cache
		 * for all the existing adds_in_progress. We should only be
		 * needing to allocate for regions_needed.
		 */
		VM_BUG_ON(resv->region_cache_count < resv->adds_in_progress);

		spin_unlock(&resv->lock);
		for (i = 0; i < to_allocate; i++) {
			trg = kmalloc(sizeof(*trg), GFP_KERNEL);
			if (!trg)
				goto out_of_memory;
			list_add(&trg->link, &allocated_regions);
		}

		spin_lock(&resv->lock);

		list_splice(&allocated_regions, &resv->region_cache);
		resv->region_cache_count += to_allocate;
	}

	return 0;

out_of_memory:
	list_for_each_entry_safe(rg, trg, &allocated_regions, link) {
		list_del(&rg->link);
		kfree(rg);
	}
	return -ENOMEM;
}