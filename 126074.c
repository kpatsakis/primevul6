follow_huge_pd(struct vm_area_struct *vma,
	       unsigned long address, hugepd_t hpd, int flags, int pdshift)
{
	WARN(1, "hugepd follow called with no support for hugepage directory format\n");
	return NULL;
}