int expand_stack_downwards(struct vm_area_struct *vma, unsigned long address)
{
	return expand_downwards(vma, address);
}