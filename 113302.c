static inline vm_fault_t handle_mm_fault(struct vm_area_struct *vma,
		unsigned long address, unsigned int flags)
{
	/* should never happen if there's no MMU */
	BUG();
	return VM_FAULT_SIGBUS;
}