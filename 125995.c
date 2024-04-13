struct resv_map *resv_map_alloc(void)
{
	struct resv_map *resv_map = kmalloc(sizeof(*resv_map), GFP_KERNEL);
	struct file_region *rg = kmalloc(sizeof(*rg), GFP_KERNEL);

	if (!resv_map || !rg) {
		kfree(resv_map);
		kfree(rg);
		return NULL;
	}

	kref_init(&resv_map->refs);
	spin_lock_init(&resv_map->lock);
	INIT_LIST_HEAD(&resv_map->regions);

	resv_map->adds_in_progress = 0;
	/*
	 * Initialize these to 0. On shared mappings, 0's here indicate these
	 * fields don't do cgroup accounting. On private mappings, these will be
	 * re-initialized to the proper values, to indicate that hugetlb cgroup
	 * reservations are to be un-charged from here.
	 */
	resv_map_set_hugetlb_cgroup_uncharge_info(resv_map, NULL, NULL);

	INIT_LIST_HEAD(&resv_map->region_cache);
	list_add(&rg->link, &resv_map->region_cache);
	resv_map->region_cache_count = 1;

	return resv_map;
}