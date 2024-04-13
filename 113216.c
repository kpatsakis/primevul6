static inline void vma_init(struct vm_area_struct *vma, struct mm_struct *mm)
{
	static const struct vm_operations_struct dummy_vm_ops = {};

	memset(vma, 0, sizeof(*vma));
	vma->vm_mm = mm;
	vma->vm_ops = &dummy_vm_ops;
	INIT_LIST_HEAD(&vma->anon_vma_chain);
}