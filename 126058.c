static unsigned long hugetlb_vm_op_pagesize(struct vm_area_struct *vma)
{
	return huge_page_size(hstate_vma(vma));
}