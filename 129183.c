ia32_install_shared_page (struct vm_area_struct *vma, unsigned long address, int *type)
{
	struct page *pg = ia32_shared_page[smp_processor_id()];
	get_page(pg);
	if (type)
		*type = VM_FAULT_MINOR;
	return pg;
}