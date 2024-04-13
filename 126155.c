resv_map_set_hugetlb_cgroup_uncharge_info(struct resv_map *resv_map,
					  struct hugetlb_cgroup *h_cg,
					  struct hstate *h)
{
#ifdef CONFIG_CGROUP_HUGETLB
	if (!h_cg || !h) {
		resv_map->reservation_counter = NULL;
		resv_map->pages_per_hpage = 0;
		resv_map->css = NULL;
	} else {
		resv_map->reservation_counter =
			&h_cg->rsvd_hugepage[hstate_index(h)];
		resv_map->pages_per_hpage = pages_per_huge_page(h);
		resv_map->css = &h_cg->css;
	}
#endif
}