void __init hugetlb_cma_check(void)
{
	if (!hugetlb_cma_size || cma_reserve_called)
		return;

	pr_warn("hugetlb_cma: the option isn't supported by current arch\n");
}