static void region_abort(struct resv_map *resv, long f, long t,
			 long regions_needed)
{
	spin_lock(&resv->lock);
	VM_BUG_ON(!resv->region_cache_count);
	resv->adds_in_progress -= regions_needed;
	spin_unlock(&resv->lock);
}