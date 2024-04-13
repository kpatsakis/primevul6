int do_munmap(struct mm_struct *mm, unsigned long start, size_t len)
{
	unsigned long end;
	struct vm_area_struct *vma, *prev, *last;

	if ((start & ~PAGE_MASK) || start > TASK_SIZE || len > TASK_SIZE-start)
		return -EINVAL;

	if ((len = PAGE_ALIGN(len)) == 0)
		return -EINVAL;

	/* Find the first overlapping VMA */
	vma = find_vma_prev(mm, start, &prev);
	if (!vma)
		return 0;
	/* we have  start < vma->vm_end  */

	/* if it doesn't overlap, we have nothing.. */
	end = start + len;
	if (vma->vm_start >= end)
		return 0;

	/*
	 * If we need to split any vma, do it now to save pain later.
	 *
	 * Note: mremap's move_vma VM_ACCOUNT handling assumes a partially
	 * unmapped vm_area_struct will remain in use: so lower split_vma
	 * places tmp vma above, and higher split_vma places tmp vma below.
	 */
	if (start > vma->vm_start) {
		int error = split_vma(mm, vma, start, 0);
		if (error)
			return error;
		prev = vma;
	}

	/* Does it split the last one? */
	last = find_vma(mm, end);
	if (last && end > last->vm_start) {
		int error = split_vma(mm, last, end, 1);
		if (error)
			return error;
	}
	vma = prev? prev->vm_next: mm->mmap;

	/*
	 * Remove the vma's, and unmap the actual pages
	 */
	detach_vmas_to_be_unmapped(mm, vma, prev, end);
	unmap_region(mm, vma, prev, start, end);

	/* Fix up all other VM information */
	remove_vma_list(mm, vma);

	return 0;
}