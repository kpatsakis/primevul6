static void record_hugetlb_cgroup_uncharge_info(struct hugetlb_cgroup *h_cg,
						struct hstate *h,
						struct resv_map *resv,
						struct file_region *nrg)
{
#ifdef CONFIG_CGROUP_HUGETLB
	if (h_cg) {
		nrg->reservation_counter =
			&h_cg->rsvd_hugepage[hstate_index(h)];
		nrg->css = &h_cg->css;
		/*
		 * The caller will hold exactly one h_cg->css reference for the
		 * whole contiguous reservation region. But this area might be
		 * scattered when there are already some file_regions reside in
		 * it. As a result, many file_regions may share only one css
		 * reference. In order to ensure that one file_region must hold
		 * exactly one h_cg->css reference, we should do css_get for
		 * each file_region and leave the reference held by caller
		 * untouched.
		 */
		css_get(&h_cg->css);
		if (!resv->pages_per_hpage)
			resv->pages_per_hpage = pages_per_huge_page(h);
		/* pages_per_hpage should be the same for all entries in
		 * a resv_map.
		 */
		VM_BUG_ON(resv->pages_per_hpage != pages_per_huge_page(h));
	} else {
		nrg->reservation_counter = NULL;
		nrg->css = NULL;
	}
#endif
}