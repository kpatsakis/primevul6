hugetlb_resv_map_add(struct resv_map *map, struct file_region *rg, long from,
		     long to, struct hstate *h, struct hugetlb_cgroup *cg,
		     long *regions_needed)
{
	struct file_region *nrg;

	if (!regions_needed) {
		nrg = get_file_region_entry_from_cache(map, from, to);
		record_hugetlb_cgroup_uncharge_info(cg, h, map, nrg);
		list_add(&nrg->link, rg->link.prev);
		coalesce_file_region(map, nrg);
	} else
		*regions_needed += 1;

	return to - from;
}