static bool has_same_uncharge_info(struct file_region *rg,
				   struct file_region *org)
{
#ifdef CONFIG_CGROUP_HUGETLB
	return rg->reservation_counter == org->reservation_counter &&
	       rg->css == org->css;

#else
	return true;
#endif
}