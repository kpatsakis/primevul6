detach_vmas_to_be_unmapped(struct mm_struct *mm, struct vm_area_struct *vma,
	struct vm_area_struct *prev, unsigned long end)
{
	struct vm_area_struct **insertion_point;
	struct vm_area_struct *tail_vma = NULL;
	unsigned long addr;

	insertion_point = (prev ? &prev->vm_next : &mm->mmap);
	do {
		rb_erase(&vma->vm_rb, &mm->mm_rb);
		mm->map_count--;
		tail_vma = vma;
		vma = vma->vm_next;
	} while (vma && vma->vm_start < end);
	*insertion_point = vma;
	tail_vma->vm_next = NULL;
	if (mm->unmap_area == arch_unmap_area)
		addr = prev ? prev->vm_end : mm->mmap_base;
	else
		addr = vma ?  vma->vm_start : mm->mmap_base;
	mm->unmap_area(mm, addr);
	mm->mmap_cache = NULL;		/* Kill the cache. */
}