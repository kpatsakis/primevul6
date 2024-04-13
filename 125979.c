get_file_region_entry_from_cache(struct resv_map *resv, long from, long to)
{
	struct file_region *nrg = NULL;

	VM_BUG_ON(resv->region_cache_count <= 0);

	resv->region_cache_count--;
	nrg = list_first_entry(&resv->region_cache, struct file_region, link);
	list_del(&nrg->link);

	nrg->from = from;
	nrg->to = to;

	return nrg;
}