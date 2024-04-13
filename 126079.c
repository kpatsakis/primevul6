static void copy_hugetlb_cgroup_uncharge_info(struct file_region *nrg,
					      struct file_region *rg)
{
#ifdef CONFIG_CGROUP_HUGETLB
	nrg->reservation_counter = rg->reservation_counter;
	nrg->css = rg->css;
	if (rg->css)
		css_get(rg->css);
#endif
}