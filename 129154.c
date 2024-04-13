ia32_install_gate_page (struct vm_area_struct *vma, unsigned long address, int *type)
{
	struct page *pg = ia32_gate_page;
	get_page(pg);
	if (type)
		*type = VM_FAULT_MINOR;
	return pg;
}