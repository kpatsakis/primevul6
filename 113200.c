static inline void vma_set_page_prot(struct vm_area_struct *vma)
{
	vma->vm_page_prot = vm_get_page_prot(vma->vm_flags);
}