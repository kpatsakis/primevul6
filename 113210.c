static inline vm_fault_t vmf_insert_page(struct vm_area_struct *vma,
				unsigned long addr, struct page *page)
{
	int err = vm_insert_page(vma, addr, page);

	if (err == -ENOMEM)
		return VM_FAULT_OOM;
	if (err < 0 && err != -EBUSY)
		return VM_FAULT_SIGBUS;

	return VM_FAULT_NOPAGE;
}